#include <algorithm>
#include <math.h>
#include <limits>
#include <unordered_set>
#include <algorithm>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_min.h>
#include "Defender.h"


double Defender::estimated_p_loot = 0;
int64_t Defender::d_init = 0; 
int64_t Defender::defender_iter_sum = 0;
int64_t Defender::current_sum_assets = 0;
int64_t Defender::sum_recovery_costs = 0;
int64_t Defender::policiesPurchased = 0;
int64_t Defender::defensesPurchased = 0;
int64_t Defender::do_nothing = 0;

int64_t Defender::sum_security_investments = 0;

uint32_t Defender::NUM_QUOTES = 0;
double Defender::MANDATORY_INVESTMENT = 0;
double Defender::DEPRECIATION = 0;
double Defender::INVESTMENT_SCALING_FACTOR = 0;


std::mt19937* Defender::gen = 0;

double Defender::ransom_b0 = 0;
double Defender::ransom_b1 = 0;
double Defender::recovery_base = 0;
double Defender::recovery_exp = 0;

std::vector<Insurer>* Defender::insurers;
std::vector<uint32_t>* Defender::alive_insurers_indices;

// Verbose bookkeeping vars
std::vector<unsigned long long> Defender::cumulative_assets;
std::vector<int> Defender::cumulative_round_policies_purchased;
std::vector<int> Defender::cumulative_round_defenses_purchased;
std::vector<int> Defender::cumulative_round_do_nothing;
int Defender::round_policies_purchased = 0;
int Defender::round_defenses_purchased = 0;
int Defender::round_do_nothing = 0;

Defender::Defender(int id_in, Params &p) : Player(p) {
    id = id_in;
    assert(id >= 0);

    double fp_assets = p.WEALTH_distribution->draw() * pow(10, 9); // Baseline params in terms of billions, so we undo here 
    assert(fp_assets < __UINT64_MAX__);
    assets = (int64_t) fp_assets;

    // initialize defenders with initial capex that will yield average posture 
    capex = (int64_t) fp_assets * p.TARGET_SECURITY_SPENDING_distribution->draw(); 
    double noise = p.POSTURE_NOISE_distribution->draw();
    posture = posture_if_investment(0, assets, capex); // No initial opex. Capex allocation above should produce desired distribution
    if (!p.sweep) { // ignore this assertion in sweep experiments
        assert(abs(posture - p.POSTURE_distribution->mean()) < 0.01); // The posture_if_investment function should produce average posture given the target security spending
    }
    posture += noise;


    if (posture < 0) {
        posture = 0;
    } else if (posture > 1) {
        posture = 1;
    }

    d_init += assets;
    current_sum_assets += assets;

    assert(Defender::d_init == Defender::current_sum_assets);
} 

void Defender::purchase_insurance_policy(Insurer* i, PolicyType p) {
    assert(assets > p.premium);
    policiesPurchased += 1;
    round_policies_purchased++;
    insured = true;
    policy = p;
    this->lose(policy.premium);
    i->sell_policy(policy);
    ins_idx = i->id;
}

void Defender::submit_claim(uint32_t loss) {
    
    assert(insured); // you should only call this function if you have an active insurance policy
    assert(ins_idx >= 0); 

    uint32_t claim_after_retention = std::max(0, ((int32_t)loss - (int32_t)policy.retention));
    assert(claim_after_retention >= 0);
    if (claim_after_retention > 0){
        if (insurers->at(ins_idx).is_alive()) {
            uint32_t amount_recovered = insurers->at(ins_idx).issue_payment(claim_after_retention);
            assert(loss >= amount_recovered);
            assert(claim_after_retention >= amount_recovered);
            assert(amount_recovered > 0);
            this->gain(amount_recovered);
        }
    }
}

void Defender::make_security_investment(uint32_t amount) {
    
    assert(amount >= 0);
    assert(amount <= assets);
    posture = posture_if_investment(amount, assets, capex);
    assert(posture >= 0);
    assert(posture <= 1);
    defensesPurchased += 1;
    round_defenses_purchased++;
    sum_security_investments += amount;
    this->lose(amount);

    // opex is twice capex spending 
    // => amount = capex + opex = capex + 2 * capex 
    // => (new) capex = amount / 3
    capex += amount / 3;
}

// Assumes that ransom payments are linear with organization size
int64_t Defender::ransom_cost(int64_t _assets) {
    assert(_assets >= 0);
    double dransom = ransom_b0 + (_assets * ransom_b1);
    int64_t ransom = std::round(dransom);
    // ransom = std::min(ransom, _assets);
    // ransom = std::max(ransom, (int64_t) 0);
    assert(std::round(ransom) >= std::round(ransom_b0));
    // assert(ransom <= _assets); // Not necessarily true! In linear ransom price, even assets = 0 will cause a ransom_b0 ransom
    return ransom;
}

// Assumes that recovery costs are a power law function of organization size
int64_t Defender::recovery_cost(int64_t _assets) {
    assert(_assets >= 0);
    double drec = recovery_base * pow(_assets, recovery_exp);
    int64_t rec = std::round(drec);
    rec = std::min(rec, _assets);
    // rec = std::max(rec, (int64_t) 0);
    assert(rec >= 0);
    // assert(rec <= _assets);
    return rec;
}

// The loss that Defender d expects to incur with investment
int64_t Defender::expected_loss_given_investment(int64_t investment, int64_t assets_, int64_t capex_, double est_p_attack) {
    int64_t expected_cost = (int64_t) (probability_of_loss(investment, assets_, capex_, est_p_attack) * cost_if_attacked(investment, assets_));
    assert(expected_cost >= 0);
    assert(expected_cost <= assets_);
    assert(expected_cost < INT64_MAX / 2); // protect against any possible overflow
    
    assert(investment <= assets_);
    assert(investment >= 0);
    assert(investment < INT64_MAX / 2); // protect against any possible overflow

    int64_t loss = investment + expected_cost;
    loss = std::min(loss, assets_);
    assert(loss >= 0);
    assert(loss <= assets_);

    return loss;
}

// yields the expected posture if a defender were to invest investment into security
// TODO add default value for expected value vs random draw?
double Defender::posture_if_investment(int64_t investment, int64_t assets_, int64_t capex_) {
    double investment_pct = (double) investment / (double) assets_;
    assert(investment_pct >= 0);
    assert(investment_pct <= 1);
        
    double capex_pct = (double) capex_ / (double) assets_;
    // assert(capex_pct <= 1); // this isn't necessarily true, since capex accumulates

    // note: this can exceed 1 because of prior capex! 
    double total_investment_pct = investment_pct + capex_pct;

    double new_posture = erf(total_investment_pct * Defender::INVESTMENT_SCALING_FACTOR);
    assert(new_posture >= 0);
    assert(new_posture <= 1);

    return new_posture;
}

// derivative of posture_if_investment with respect to amount
double Defender::d_posture_if_investment(int64_t investment, int64_t assets_, int64_t capex_) {
    double s = Defender::INVESTMENT_SCALING_FACTOR;
    double power = -1 * (pow((double) s, 2) * pow((double) capex_ + investment, 2)) / pow((double) assets_, 2);
    return (2 * s * exp(power)) / ((double) assets_ * sqrt(M_PI));
}

// // second derivative of posture_if_investment with respect to amount
// double Defender::d_d_posture_if_investment(int64_t investment) {
//     double s = p.INVESTMENT_SCALING_FACTOR_distribution->draw();
//     double power = -1 * (pow((double) s, 2) * pow((double) capex + investment, 2)) / pow((double) this->assets, 2);
//     double numerator = -4 * pow(s, 3) * (capex + investment) * exp(power);
//     double denominator = sqrt(M_PI) * pow(this->assets, 3);
    
//     return numerator / denominator; 
// }

int64_t Defender::cost_if_attacked(int64_t investment, int64_t assets_) {
    assert(investment <= assets_);
    int64_t rans = ransom_cost(assets_ - investment);
    int64_t recovery = recovery_cost(assets_ - investment);
    assert(rans > 0);
    assert(recovery >= 0);
    int64_t cost = rans + recovery;
    assert(cost > 0);
    if (cost > assets_) {
        cost = assets_;
    }
    return cost;
}

// derivative of cost_if_attacked with respect to investment
double Defender::d_cost_if_attacked(int64_t investment, int64_t assets_) {
   double d_ransom = -1 * Defender::ransom_b1;
   double d_recovery = -1 * Defender::recovery_base * Defender::recovery_exp * pow(assets_ - investment, Defender::recovery_exp - 1);
   return d_ransom + d_recovery;
}

// // second derivative of cost_if_attacked with respect to investment
// double Defender::d_d_cost_if_attacked(int64_t investment) {
//    return recovery_base * (recovery_exp - 1) * (recovery_base) * pow(this->assets - investment, recovery_exp - 2);
// }

double Defender::probability_of_loss(int64_t investment, int64_t assets_, int64_t capex_, double est_p_attack) {
    double prob_loss = est_p_attack * (1 - posture_if_investment(investment, assets_, capex_));
    assert(prob_loss >= 0);
    assert(prob_loss <= 1);
    return prob_loss;
}

// derivative of probability_of_loss with respect to investment
double Defender::d_probability_of_loss(int64_t investment, int64_t assets_, int64_t capex_, double est_p_attack) {
    double d_prob_loss = -1 * est_p_attack *  d_posture_if_investment(investment, assets_, capex_);
    return d_prob_loss;
}

// // second derivative of probability_of_loss with respect to investment
// double Defender::d_d_probability_of_loss(int64_t investment) {
//     double d_d_prob_loss = -1 * Defender::estimated_probability_of_attack *  d_d_posture_if_investment(investment);
//     return d_d_prob_loss;
// }

// Returns a boolean stating if expected_loss has a minimum in the range [0, assets]
bool Defender::expected_loss_contains_minimum(int64_t investment, int64_t assets_, int64_t capex_, double est_p_attack) {
    // expected_loss can be either convex unimodal, or monotonic
    // if it is convex unimodal, there is a minimum, and the gsl_find_minimum will find it
    // else if it is monotonic, then the slope of expected_cost will be greater than or equal to zero
    // I.e. we find the derivative of expected_cost with respect to investment
    // f(x) = investment + probability_of_loss(investment) * cost_if_attacked(investment) // investment = x
    double d_expected_loss_wrt_investment = 1 + (d_probability_of_loss(investment, assets_, capex_, est_p_attack) * cost_if_attacked(investment, assets_)) + (probability_of_loss(investment, assets_, capex_, est_p_attack) * d_cost_if_attacked(investment, assets_)); // derivative product rule
    bool test1 = d_expected_loss_wrt_investment < 0? true : false;
    
    return test1;
}

struct opt_params{
    int64_t assets;
    int64_t capex;
    double est_p_attack;
};

double Defender::gsl_expected_loss_wrapper(double x, void * params) {
    struct opt_params * p = (struct opt_params *)params;
    int64_t assets_ = (p->assets);
    int64_t capex_ = (p->capex);
    double est_p_attack = (p->est_p_attack);
    return (double) Defender::expected_loss_given_investment(x, assets_, capex_, est_p_attack);
}

double Defender::gsl_find_minimum() {

    // Detect if expected_cost is monotonic instead of convex unimodal
    if (!expected_loss_contains_minimum(0, assets, capex, defender_specific_estimated_p_attack)) {
        // expected_cost is monotonic, so the minimum is when investment=0
        return 0;
    }

    int status;
    int iter = 0, max_iter = 100;
    const gsl_min_fminimizer_type *T;
    gsl_min_fminimizer *s;
    double m = assets / 100.0; // initial guess
    double a = 0.0; // lower bound
    double b = assets; // upper bound
    gsl_function F;

    opt_params p;
    p.assets = assets;
    p.capex = capex;
    p.est_p_attack = defender_specific_estimated_p_attack;

    F.function = &gsl_expected_loss_wrapper; 
    F.params = &p;

    T = gsl_min_fminimizer_brent;
    s = gsl_min_fminimizer_alloc (T);

    // disable default gsl error handler
    // but this means we have to actually check the return status of library routines
    gsl_set_error_handler_off();

    status = gsl_min_fminimizer_set(s, &F, m, a, b);

    if (status != 0) {
        // gsl_min_fminimizer_set failed
        // likely no minimum for gsl to find
        return 0;
    }
    


    // std::cout << "using" <<  gsl_min_fminimizer_name(s) << " method" << std::endl;
    // printf("%5s [%9s, %9s] %9s %9s\n", "iter", "lower", "upper", "min", "err(est)");
    // printf("%5d [%.7f, %.7f] %.7f %.7f\n", iter, a, b, m, b - a);

    double absolute_error_tolerance = 1; // we convert to int type anyway, so no need to optimize beyond 1
    double relative_error_tolerance = 0; // we only care about absolute error so we set this term to 0

    do {
        iter++;
        status = gsl_min_fminimizer_iterate (s);

        m = gsl_min_fminimizer_x_minimum (s);
        a = gsl_min_fminimizer_x_lower (s);
        b = gsl_min_fminimizer_x_upper (s);

        status = gsl_min_test_interval (a, b, absolute_error_tolerance, relative_error_tolerance);

        // if (status == GSL_SUCCESS)
        //     printf("Converged:\n");
        // printf("%5d [%.7f, %.7f] %.7f %.7f\n", iter, a, b, m, b - a);
    
    } while(status == GSL_CONTINUE && iter < max_iter);

    gsl_min_fminimizer_free(s);

    return m;
}

void Defender::shop_around_for_insurance_policies(Insurer** best_insurer, PolicyType &best_policy, bool &insurable) {

    std::uniform_int_distribution<> alive_insurers_indices_dist(0, alive_insurers_indices->size() -1);
    
    // pick insurers for quotes
    // Probably a more performant way of doing this 
    std::unordered_set<uint32_t> insurer_indices;
    while (insurer_indices.size() < NUM_QUOTES && insurer_indices.size() < alive_insurers_indices->size()) {
        uint32_t alive_insurer_index = alive_insurers_indices_dist(*gen);
        uint32_t insurer_index = alive_insurers_indices->at(alive_insurer_index);
        insurer_indices.insert(insurer_index);
    }
    
    // Iterate through random insurers and request quotes
    for (const auto& j : insurer_indices) {
        Insurer* i = &insurers->at(j);
        assert(i->is_alive());
        
        double noise = p.POSTURE_NOISE_distribution->draw();
        double estimated_posture = posture + noise;

        // adding noise might cause posture to go out of bounds 
        estimated_posture = std::max(0.0, estimated_posture);
        estimated_posture = std::min(1.0, estimated_posture);

        assert(estimated_posture >= 0);
        assert(estimated_posture <= 1);

        PolicyType policy = i->provide_a_quote(assets, estimated_posture); 
        
        if (policy.premium == 0 ||  policy.premium >= assets) {
            // Coverage not available
            continue;
        } else {
            assert(policy.premium > 0); 
            assert(policy.retention > 0);

            if (policy.premium < best_policy.premium) { // works because retention is a scaled version of premium. So best premium = best policy
                insurable = true;
                best_policy = policy;
                *best_insurer = i;
            }      
        }
    }
}

int64_t Defender::expected_loss_given_insurance(PolicyType &policy, double posture, double est_p_attack) {

    double p_L_hat = est_p_attack * (1 - posture);
    assert(p_L_hat >= 0);
    assert(p_L_hat <= 1);

    assert(policy.premium > 0 ); 
    assert(policy.retention > 0);
    int64_t expected_loss_with_insurance = policy.premium + (int64_t) (p_L_hat * policy.retention);
    assert(expected_loss_with_insurance >= 0);

    return expected_loss_with_insurance;
}


void Defender::mandatory_security_experiment() {
    // Some games will require Defenders to make a mandatory security investment
    // If this is the case, do it before calculating remaining optimal strategies or requesting insurance policies
    double inv_pct = p.MANDATORY_INVESTMENT_distribution->draw();
    assert(inv_pct >= 0);
    assert(inv_pct <= 1);
    int64_t mandatory_investment = assets * inv_pct;
    // assert(mandatory_investment > 0); // commented out. What if assets=94 and inv_pct = 0.01?
    assert(mandatory_investment <= assets);
    make_security_investment(mandatory_investment);
    defensesPurchased--;
    round_defenses_purchased--;

    default_experiment();
}

void Defender::mandatory_insurance_experiment() {
    // 1. Get insurance policy from insurer
    Insurer* best_insurer = nullptr;
    PolicyType best_policy;
    best_policy.premium = std::numeric_limits<int64_t>::max();
    bool insurable = false;

    shop_around_for_insurance_policies(&best_insurer, best_policy, insurable);
    if (insurable) {
        assert(best_insurer != nullptr);
        purchase_insurance_policy(best_insurer, best_policy);
    }

    // 2. Find optimum security investment
    int64_t optimal_investment = (int64_t) gsl_find_minimum();
    assert(optimal_investment >= 0);
    assert(optimal_investment <= assets);

    if (optimal_investment > 0) {
        make_security_investment(optimal_investment);
    } else {
        assert(optimal_investment == 0);
        do_nothing++;
        round_do_nothing++;
    }
}

void Defender::default_experiment() {

    // 1. Get insurance policy from insurer
    Insurer* best_insurer = nullptr;
    PolicyType best_policy;
    best_policy.premium = std::numeric_limits<int64_t>::max();
    bool insurable = false;
    shop_around_for_insurance_policies(&best_insurer, best_policy, insurable);

    int64_t expected_loss_with_insurance = INT64_MAX;
    if (insurable) {
        assert(best_insurer != nullptr);
        expected_loss_with_insurance = expected_loss_given_insurance(best_policy, posture, defender_specific_estimated_p_attack);
    }

    // 2. Find optimum security investment
    int64_t optimal_investment = (int64_t) gsl_find_minimum();
    assert(optimal_investment >= 0);
    assert(optimal_investment <= assets);
    
    int64_t expected_loss_with_optimal_investment = expected_loss_given_investment(optimal_investment, assets, capex, defender_specific_estimated_p_attack);
    assert(expected_loss_with_optimal_investment >= 0);
    assert(expected_loss_with_optimal_investment <= assets);

    // TODO consider possibility that players can choose both
    // TODO consider cases where insurer tells defender how much to invest 

    if (insurable && (expected_loss_with_insurance < expected_loss_with_optimal_investment) ) {
        purchase_insurance_policy(best_insurer, best_policy);
    } else if (optimal_investment > 0) {
        make_security_investment(optimal_investment);
    } else {
        assert(optimal_investment == 0);
        do_nothing++;
        round_do_nothing++;
    }
}

void Defender::choose_security_strategy() {

    if (p.MANDATORY_INVESTMENT_distribution->mean() > 0 ) {
        assert(p.mandatory_insurance == false); // Let's make sure we don't try to run two expreiments at once
        mandatory_security_experiment();
    } else if (p.mandatory_insurance) {
        assert(p.MANDATORY_INVESTMENT_distribution->mean() == 0); // Let's make sure we don't try to run two expreiments at once
        mandatory_insurance_experiment();
    } else {
        assert(p.mandatory_insurance == false); 
        assert(p.MANDATORY_INVESTMENT_distribution->mean() == 0);
        default_experiment(); 
    }
}

void Defender::security_depreciation() {
    // the value of previous opex spending depreciates to zero after it is spent (by definition)
    capex = capex * (1 - Defender::DEPRECIATION);
    posture = posture_if_investment(0, assets, capex); // 0 invested in opex (for now)
}

void Defender::perform_market_analysis(std::vector<Defender> &defenders, double last_round_p_loot) {
    // Defenders don't have the same visibility as the insurers but still can make some predictions about risk.
    
    Defender::estimated_p_loot = last_round_p_loot;
    assert(Defender::estimated_p_loot >= 0);
    assert(Defender::estimated_p_loot <= 1);

    for (uint i=0; i<defenders.size(); i++) {
        if (defenders[i].is_alive()) {
            double dsepa = Defender::estimated_p_loot / (1 - defenders[i].posture);
            dsepa = std::min(dsepa, 1.0);
            dsepa = std::max(dsepa, 0.0);
            defenders[i].defender_specific_estimated_p_attack = dsepa;
        }
    }
}

void Defender::lose(int64_t loss) {
    Player::lose(loss);
    defender_iter_sum -= loss;
    current_sum_assets -= loss;
}

void Defender::gain(int64_t gain) {
    Player::gain(gain);
    defender_iter_sum += gain;
    current_sum_assets += gain;
}

void Defender::reset() {
    estimated_p_loot = 0;
    d_init = 0;
    defender_iter_sum = 0;
    current_sum_assets = 0; 
    sum_security_investments = 0;

    policiesPurchased = 0;
    defensesPurchased = 0;
    do_nothing = 0;
    sum_recovery_costs = 0;
    cumulative_assets.clear();

    cumulative_round_policies_purchased.clear();
    cumulative_round_defenses_purchased.clear();
    cumulative_round_do_nothing.clear();
    round_policies_purchased = 0;
    round_defenses_purchased = 0;
    round_do_nothing = 0;

    ransom_b0 = 0;
    ransom_b1 = 0;
    recovery_base = 0;
    recovery_exp = 0;

    NUM_QUOTES = 0;
    MANDATORY_INVESTMENT = 0;
    DEPRECIATION = 0;
    INVESTMENT_SCALING_FACTOR = 0;
    gen = nullptr;

    insurers = nullptr;
    alive_insurers_indices = nullptr;
}