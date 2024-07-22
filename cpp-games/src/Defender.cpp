#include <algorithm>
#include <math.h>
#include <limits>
#include <unordered_set>
#include <algorithm>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_min.h>
#include "Defender.h"






///////////////////////
// double fn1 (double x, void * params)
// {
//   (void)(params); /* avoid unused parameter warning */
//   return cos(x) + 1.0;
// }
////////////



double Defender::estimated_probability_of_attack = 0;
int64_t Defender::d_init = 0; 
int64_t Defender::defender_iter_sum = 0;
int64_t Defender::current_sum_assets = 0;
int64_t Defender::sum_recovery_costs = 0;
int64_t Defender::policiesPurchased = 0;
int64_t Defender::defensesPurchased = 0;
int64_t Defender::do_nothing = 0;

int64_t Defender::sum_security_investments = 0;

uint32_t Defender::NUM_QUOTES = 0;

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
    posture = posture_if_investment(0); // No initial opex. Capex allocation above should produce desired distribution
    assert(abs(posture - 0.28) < 0.01); // TODO make sure setup is correct using params
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
    posture = posture_if_investment(amount);
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
    double dransom = ransom_b0 + (_assets * ransom_b1);
    int64_t ransom = (int64_t) dransom;
    ransom = std::min(ransom, _assets);
    assert(ransom >= 0);
    assert(ransom <= _assets);
    return ransom;
}

// Assumes that recovery costs are a power law function of organization size
int64_t Defender::recovery_cost(int64_t _assets) {
    double drec = recovery_base * pow(_assets, recovery_exp);
    int64_t rec = (int64_t) drec;
    rec = std::min(rec, _assets);
    assert(rec >= 0);
    assert(rec <= _assets);
    return rec;
}

// The loss that Defender d expects to incur with investment
int64_t Defender::expected_loss(int64_t investment) {
    int64_t expected_cost = (int64_t) (probability_of_loss(investment) * cost_if_attacked(investment));
    assert(expected_cost >= 0);
    assert(expected_cost <= assets);
    assert(expected_cost < INT64_MAX / 2); // protect against any possible overflow
    
    assert(investment <= assets);
    assert(investment >= 0);
    assert(investment < INT64_MAX / 2); // protect against any possible overflow

    int64_t loss = investment + expected_cost;
    assert(loss >= 0);
    assert(loss <= assets);

    return loss;
}

// same as expected_loss but formatted using double data types for gsl
double gsl_expected_loss(double x, void * params) {
    // (void)(params); /* avoid unused parameter warning */
    Defender* self = static_cast<Defender*>(params);
    int64_t investment = x;
    return (double) self->expected_loss(investment);
    // return (double) investment * investment + investment; // TODO delete...just for testing 
}

// double gsl_expected_loss_wrapper(double x, void* params) {
//     //     Defender* self = static_cast<Defender*>(params);
//     //     return self->gsl_expected_loss(x);
//     // }

// yields the expected posture if a defender were to invest investment into security
// TODO add default value for expected value vs random draw?
double Defender::posture_if_investment(int64_t investment) {
    double investment_pct = (double) investment / (double) this->assets;
    assert(investment_pct >= 0);
    assert(investment_pct <= 1);
    
    double investment_scaling_factor = p.INVESTMENT_SCALING_FACTOR_distribution->draw();
    
    double capex_pct = (double) capex / (double) this->assets;
    assert(capex_pct >= 0);
    // assert(capex_pct <= 1); // this isn't necessarily true, since capex accumulates

    // note: this can exceed 1 because of prior capex! 
    double total_investment_pct = investment_pct + capex_pct;

    double new_posture = erf(total_investment_pct * investment_scaling_factor);
    assert(new_posture >= 0);
    assert(new_posture <= 1);

    return new_posture;
}

// derivative of posture_if_investment with respect to amount
double Defender::d_posture_if_investment(int64_t investment) {
    double s = p.INVESTMENT_SCALING_FACTOR_distribution->draw();
    double power = -1 * (pow((double) s, 2) * pow((double) capex + investment, 2)) / pow((double) this->assets, 2);
    return (2 * s * exp(power)) / ((double) assets * sqrt(M_PI));
}

// second derivative of posture_if_investment with respect to amount
double Defender::d_d_posture_if_investment(int64_t investment) {
    double s = p.INVESTMENT_SCALING_FACTOR_distribution->draw();
    double power = -1 * (pow((double) s, 2) * pow((double) capex + investment, 2)) / pow((double) this->assets, 2);
    double numerator = -4 * pow(s, 3) * (capex + investment) * exp(power);
    double denominator = sqrt(M_PI) * pow(this->assets, 3);
    
    return numerator / denominator; 
}

int64_t Defender::cost_if_attacked(int64_t investment) {
    assert(investment <= assets);
    int64_t rans = ransom_cost(this->assets - investment);
    int64_t recovery = recovery_cost(this->assets - investment);
    assert(rans > 0);
    assert(recovery >= 0);
    int64_t cost = rans + recovery;
    assert(cost > 0);
    if (cost > assets) {
        cost = assets;
    }
    return cost;
}

// derivative of cost_if_attacked with respect to investment
double Defender::d_cost_if_attacked(int64_t investment) {
   double d_ransom = -1 * ransom_b1;
   double d_recovery = -1 * recovery_base * recovery_exp * pow(this->assets - investment, recovery_exp - 1);
   return d_ransom + d_recovery;
}

// second derivative of cost_if_attacked with respect to investment
double Defender::d_d_cost_if_attacked(int64_t investment) {
   return recovery_base * (recovery_exp - 1) * (recovery_base) * pow(this->assets - investment, recovery_exp - 2);
}

double Defender::probability_of_loss(int64_t investment) {
    double prob_loss = Defender::estimated_probability_of_attack * (1 - posture_if_investment(investment));
    assert(prob_loss >= 0);
    assert(prob_loss <= 1);
    return prob_loss;
}

// derivative of probability_of_loss with respect to investment
double Defender::d_probability_of_loss(int64_t investment) {
    double d_prob_loss = -1 * Defender::estimated_probability_of_attack *  d_posture_if_investment(investment);
    return d_prob_loss;
}

// second derivative of probability_of_loss with respect to investment
double Defender::d_d_probability_of_loss(int64_t investment) {
    double d_d_prob_loss = -1 * Defender::estimated_probability_of_attack *  d_d_posture_if_investment(investment);
    return d_d_prob_loss;
}

// Newton-Raphson method for finding the root of the derivative of expected loss
double Defender::find_optimal_investment(){

    // provide an initial guess
    // Start with guess=0 so that successively better guesses always increment *if* there is an optimum
    // so that we can catch cases that will not converge (guess < 0) before they cause problems
    int64_t guess = assets / 100; 
    int64_t last_guess = assets / 100;
    do {
        
        int64_t investment = guess;
        last_guess = guess;

        // Shouldn't this be cost_if_attacked(assets - investment) and not cost_if_attacked(investment)?
        // No. cost_if_attacked already does this subtraction for you.
        // Somewhat confusing, I know.
        // FYI: f(x) = investment + probability_of_loss(investment) * cost_if_attacked(investment)
        double d_fx = 1 + (d_probability_of_loss(investment) * cost_if_attacked(investment)) + (probability_of_loss(investment) * d_cost_if_attacked(investment)); // derivative product rule
        
        double t1 = d_d_probability_of_loss(investment) * cost_if_attacked(investment);
        double t2 = 2 * d_probability_of_loss(investment) * d_cost_if_attacked(investment);
        double t3 = probability_of_loss(investment) * d_d_cost_if_attacked(investment);
        double d_d_fx = t1 + t2 + t3;
        guess = (int64_t) (last_guess - (d_fx / d_d_fx));

        if (guess < 0 || guess > this->assets) {
            // It is possible that the expected loss function does not have a minimum 
            // meaning that Newton-Raphson does not converge.
            // In this case, the optimal investment is either none or all of assets
            // In these situations it is better to invest nothing and hope for the best 
            // rather than investing all of one's assets into security
            guess = 0; 
            break;
        }
    
    // Compare against the last *two* guesses to avoid getting stuck in oscillatory loops
    // }  while (guess != last_guess && guess != last_last_guess);
    } while (abs(guess - last_guess) > 1);

    int64_t optimal_investment = guess;
    assert(optimal_investment >= 0);
    assert(optimal_investment <= assets);
    return optimal_investment;
}


double Defender::gsl_find_minimum() {
    int status;
    int iter = 0, max_iter = 100;
    const gsl_min_fminimizer_type *T;
    gsl_min_fminimizer *s;
    double m = assets / 100.0; // initial guess
    double a = 0.0; // lower bound
    //   double m_expected = assets / 100.0; // from gsl tutorial // just for testing demonstration purposes
    double b = assets; // upper bound
    gsl_function F;

    // F.function = &fn1;
    F.function = &gsl_expected_loss;
    F.params = 0;

    T = gsl_min_fminimizer_brent;
    s = gsl_min_fminimizer_alloc (T);
    gsl_min_fminimizer_set (s, &F, m, a, b);

    printf ("using %s method\n",
            gsl_min_fminimizer_name (s));

    printf ("%5s [%9s, %9s] %9s %10s %9s\n",
            "iter", "lower", "upper", "min",
            "err", "err(est)");

    //   printf ("%5d [%.7f, %.7f] %.7f %+.7f %.7f\n",
    //           iter, a, b,
    //           m, m - m_expected, b - a);

    double absolute_error_tolerance = 1; // we convert to int type anyway, so no need to optimize beyond 1
    double relative_error_tolerance = 0; // we only care about absolute error so we set this term to 0

    do
    {
        iter++;
        status = gsl_min_fminimizer_iterate (s);

        m = gsl_min_fminimizer_x_minimum (s);
        a = gsl_min_fminimizer_x_lower (s);
        b = gsl_min_fminimizer_x_upper (s);

        status = gsl_min_test_interval (a, b, absolute_error_tolerance, relative_error_tolerance);

        if (status == GSL_SUCCESS)
        printf ("Converged:\n");

    //   printf ("%5d [%.7f, %.7f] "
    //           "%.7f %+.7f %.7f\n",
    //           iter, a, b,
    //           m, m - m_expected, b - a);
    }
    while (status == GSL_CONTINUE && iter < max_iter);

    gsl_min_fminimizer_free (s);

    return m;
}

void Defender::security_depreciation() {
    // the value of previous opex spending depreciates to zero after it is spent (by definition)
    double DEPRECIATION = p.DEPRECIATION_distribution->draw();
    capex = capex * (1 - DEPRECIATION);
    posture = posture_if_investment(0); // 0 invested in opex (for now)
}

void Defender::choose_security_strategy() {

    double p_A_hat = estimated_probability_of_attack;
    assert(p_A_hat >= 0);
    assert(p_A_hat <= 1);

    double p_L_hat = p_A_hat * (1 - posture);
    assert(p_L_hat >= 0);
    assert(p_L_hat <= 1);

    // 1. Get insurance policy from insurer
    std::uniform_int_distribution<> alive_insurers_indices_dist(0, alive_insurers_indices->size() -1);
    
    // pick insurers for quotes
    // Probably a more performant way of doing this 
    std::unordered_set<uint32_t> insurer_indices;
    while (insurer_indices.size() < NUM_QUOTES && insurer_indices.size() < alive_insurers_indices->size()) {
        uint32_t alive_insurer_index = alive_insurers_indices_dist(*gen);
        uint32_t insurer_index = alive_insurers_indices->at(alive_insurer_index);
        insurer_indices.insert(insurer_index);
    }

    Insurer* best_insurer = nullptr;
    PolicyType best_policy;
    best_policy.premium = std::numeric_limits<int64_t>::max();
    bool insurable = false;
    
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
                best_insurer = i;
            }      
        }
    }

    int64_t expected_loss_with_insurance = INT64_MAX;
    if (insurable) {
        assert(best_policy.premium > 0 ); 
        assert(best_policy.retention > 0);
        assert(best_insurer != nullptr);
        expected_loss_with_insurance = best_policy.premium + (int64_t) (p_L_hat * best_policy.retention);
        assert(expected_loss_with_insurance >= 0);
    }
    
    // 2. Find optimum security investment
    // int64_t optimal_investment = gsl_find_minimum();
    int64_t optimal_investment = (int64_t) find_optimal_investment();
    assert(optimal_investment >= 0);
    assert(optimal_investment <= assets);
    
    int64_t expected_loss_with_optimal_investment = expected_loss(optimal_investment);

    
    // int64_t expected_cost_if_attacked_at_optimal_investment = ransom_cost(assets - optimal_investment) + recovery_cost(assets - optimal_investment);
    // assert(expected_cost_if_attacked_at_optimal_investment >= 0);
        
    // double p_loss_with_optimal_investment = estimated_probability_of_attack * (1 - posture_if_investment(optimal_investment));
    // assert(p_loss_with_optimal_investment <= 1);
    // assert(p_loss_with_optimal_investment >= 0);
    
    // double expected_loss_with_optimal_investment = optimal_investment +  p_loss_with_optimal_investment * expected_cost_if_attacked_at_optimal_investment;
    // assert(expected_loss_with_optimal_investment >= 0);

    // TODO consider possibility that players can choose both
    // TODO consider case where insurer requires 1% investment 
    // TODO consider cases where insurer tells defender how much to invest 
    if (insurable && expected_loss_with_insurance < expected_loss_with_optimal_investment) {
        purchase_insurance_policy(best_insurer, best_policy);
    } else if (optimal_investment > 0) {
        make_security_investment(optimal_investment);
    } else {
        assert(optimal_investment == 0);
        do_nothing++;
        round_do_nothing++;
    }
}

void Defender::perform_market_analysis(double last_round_attack_pct) {
    // Defenders don't have the same visibility as the insurers but still can make some predictions about risk.
    Defender::estimated_probability_of_attack = last_round_attack_pct;
    assert(Defender::estimated_probability_of_attack >= 0);
    assert(Defender::estimated_probability_of_attack <= 1);
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
    estimated_probability_of_attack = 0;
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
    gen = nullptr;

    insurers = nullptr;
    alive_insurers_indices = nullptr;
}


