#include <algorithm>
#include <math.h>
#include <limits>
#include <unordered_set>
#include <algorithm>
#include "Defender.h"

double Defender::estimated_probability_of_attack = 0;
int64_t Defender::d_init = 0; 
int64_t Defender::defender_iter_sum = 0;
int64_t Defender::current_sum_assets = 0;
int64_t Defender::sum_recovery_costs = 0;
int64_t Defender::policiesPurchased = 0;
int64_t Defender::defensesPurchased = 0;
int64_t Defender::sum_security_investments = 0;
std::vector<unsigned long long> Defender::cumulative_assets;

uint32_t Defender::NUM_QUOTES = 0;

std::mt19937* Defender::gen = 0;

double Defender::ransom_b0 = 0;
double Defender::ransom_b1 = 0;
double Defender::recovery_base = 0;
double Defender::recovery_exp = 0;

Defender::Defender(int id_in, Params &p, std::vector<Insurer>& _insurers) : Player(p) {
    id = id_in;
    assert(id >= 0);

    insurers = &_insurers;

    double fp_assets = p.WEALTH_distribution->draw() * pow(10, 6); // In terms of thousands. Baseline params in terms of billions. 
    assert(fp_assets < __UINT32_MAX__);
    assets = (uint32_t) fp_assets;

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
    sum_security_investments += amount;
    this->lose(amount);

    // opex is twice capex spending 
    // => amount = capex + opex = capex + 2 * capex 
    // => (new) capex = amount / 3
    capex += amount / 3;
}

// Assumes that ransom payments are linear with organization size
// TODO rename to ransom_cost
long long Defender::ransom(int _assets) {
    return ransom_b0 + (_assets * ransom_b1);
}

// Assumes that recovery costs are a power law function of organization size
long long Defender::recovery_cost(int _assets) {
    return recovery_base * pow(_assets, recovery_exp);
}

// yields the expected posture if a defender were to invest investment into security
// TODO add default value for expected value vs random draw?
double Defender::posture_if_investment(int64_t investment) {
    double investment_pct = (double) investment / (double) this->assets;
    double investment_scaling_factor = p.INVESTMENT_SCALING_FACTOR_distribution->draw();
    
    double capex_pct = (double) capex / (double) this->assets;
    double total_investment_pct = investment_pct + capex_pct;

    return erf(total_investment_pct * investment_scaling_factor);
}

// derivative of posture_if_investment with respect to amount
double Defender::d_posture_if_investment(int64_t investment) {
    double s = p.INVESTMENT_SCALING_FACTOR_distribution->draw();
    double power = -1 * (pow((double) s, 2) * pow((double) capex + investment, 2)) / pow((double) this->assets, 2);
    return (2 * s * exp(power)) / ((double) assets * sqrt(M_PI));
}

double Defender::d_d_posture_if_investment(int64_t investment) {
    double s = p.INVESTMENT_SCALING_FACTOR_distribution->draw();
    double power = -1 * (pow((double) s, 2) * pow((double) capex + investment, 2)) / pow((double) this->assets, 2);
    double numerator = -4 * pow(s, 3) * (capex + investment) * exp(power);
    double denominator = sqrt(M_PI) * pow(this->assets, 3);
    
    return numerator / denominator; 
}

int64_t Defender::cost_if_attacked(int64_t investment) {
    double rans = ransom(this->assets - investment);
    double recovery = recovery_cost(this->assets - investment);
    assert(rans > 0);
    assert(recovery >= 0);
    return rans + recovery;
}

// derivative of posture_if_investment with respect to investment
double Defender::d_cost_if_attacked(int64_t investment) {
   double d_ransom = -1 * ransom_b1;
   double d_recovery = -1 * recovery_base * recovery_exp * pow(this->assets - investment, recovery_exp - 1);
   return d_ransom + d_recovery;
}

// second derivative of posture_if_investment with respect to investment
double Defender::d_d_cost_if_attacked(int64_t investment) {
   return recovery_base * (recovery_exp - 1) * (recovery_base) * pow(this->assets - investment, recovery_exp - 2);
}

double Defender::probability_of_loss(int64_t investment) {
    double prob_loss = Defender::estimated_probability_of_attack * (1 - posture_if_investment(investment));
    assert(prob_loss >= 0);
    assert(prob_loss <= 1);
    return prob_loss;
}

double Defender::d_probability_of_loss(int64_t investment) {
    double d_prob_loss = -1 * Defender::estimated_probability_of_attack *  d_posture_if_investment(investment);
    return d_prob_loss;
}

double Defender::d_d_probability_of_loss(int64_t investment) {
    double d_d_prob_loss = -1 * Defender::estimated_probability_of_attack *  d_d_posture_if_investment(investment);
    return d_d_prob_loss;
}

double Defender::find_optimal_investment(){

    // Newton-Raphson method for finding the root of the derivative
    int64_t guess = std::max((int64_t) 1, (int64_t) ((double) assets * 0.05)); // Provide an initial guess
    assert(guess > 0);
    int64_t last_guess = -INT32_MAX, last_last_guess = -INT32_MAX; 
    do {
        
        int64_t investment = guess;
        if (investment < 0 || investment > this->assets) {
            // It is possible that the expected loss function does not have a minimum 
            // meaning that Newton-Raphson does not converge.
            // In this case, the optimal investment is either none or all of assets
            // In these situations it is better to invest nothing and hope for the best 
            // rather than investing all of one's assets into security
            guess = 0; 
            break;
        }

        last_last_guess = last_guess;
        last_guess = guess;

        double d_fx = 1 + (d_probability_of_loss(investment) * cost_if_attacked(investment)) + (probability_of_loss(investment) * d_cost_if_attacked(investment)); // multiplication rule
        
        double t1 = d_d_probability_of_loss(investment) * cost_if_attacked(investment);
        double t2 = 2 * d_probability_of_loss(investment) * d_cost_if_attacked(investment);
        double t3 = probability_of_loss(investment) * d_d_cost_if_attacked(investment);
        double d_d_fx = t1 + t2 + t3;
        guess = (int64_t) (last_guess - (d_fx / d_d_fx));
    
    // Compare against the last *two* guesses to avoid getting stuck in oscillatory loops
    }  while (guess != last_guess && guess != last_last_guess);

    int64_t optimal_investment = guess;
    assert(optimal_investment >= 0);
    assert(optimal_investment <= assets);
    return optimal_investment;
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
    // TODO what if this is picking dead insurers...?
    // TODO TODO TODO 
    std::uniform_int_distribution<> insurer_indices_dist(0, insurers->size()-1);
    
    // pick insurers for quotes
    // Probably a more performant way of doing this 
    std::unordered_set<unsigned int> insurer_indices;
    while (insurer_indices.size() < NUM_QUOTES && insurer_indices.size() < insurers->size()) {
        insurer_indices.insert(insurer_indices_dist(*gen));
    }

    Insurer* best_insurer = nullptr;
    PolicyType best_policy;
    best_policy.premium = std::numeric_limits<int64_t>::max();
    bool insurable = false;
    
    for (const auto& j : insurer_indices) {
        Insurer* i = &insurers->at(j);
        
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

    double expected_loss_with_insurance = std::numeric_limits<double>::infinity();
    if (insurable) {
        assert(best_policy.premium > 0 ); 
        assert(best_policy.retention > 0);
        assert(best_insurer != nullptr);
        expected_loss_with_insurance = best_policy.premium + (p_L_hat * best_policy.retention);
        assert(expected_loss_with_insurance >= 0);
    }
    
    // 2. Find optimum security investment
    int64_t  optimal_investment = find_optimal_investment();
    assert(optimal_investment >= 0);
    assert(optimal_investment <= assets);
    
    int64_t expected_cost_if_attacked_at_optimal_investment = ransom(assets - optimal_investment) + recovery_cost(assets - optimal_investment);
    assert(expected_cost_if_attacked_at_optimal_investment >= 0);
        
    double p_loss_with_optimal_investment = estimated_probability_of_attack * (1 -posture_if_investment(optimal_investment));
    assert(p_loss_with_optimal_investment <= 1);
    assert(p_loss_with_optimal_investment >= 0);
    
    double expected_loss_with_optimal_investment = optimal_investment +  p_loss_with_optimal_investment * expected_cost_if_attacked_at_optimal_investment;
    assert(expected_loss_with_optimal_investment >= 0);

    // TODO consider possibility that players can choose both
    // TODO consider case where insurer requires 1% investment 
    // TODO consider cases where insurer tells defender how much to invest 
    if (insurable && expected_loss_with_insurance < expected_loss_with_optimal_investment) {
        purchase_insurance_policy(best_insurer, best_policy);
    } else {
        make_security_investment(optimal_investment);
    }
}

void Defender::perform_market_analysis(int prevRoundAttacks, int num_current_defenders) {
    // Defenders don't have the same visibility as the insurers but still can make some predictions about risk.
    Defender::estimated_probability_of_attack = std::min(1.0, (prevRoundAttacks * 1.0)/(num_current_defenders * 1.0));
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
    sum_recovery_costs = 0;
    cumulative_assets.clear();

    ransom_b0 = 0;
    ransom_b1 = 0;
    recovery_base = 0;
    recovery_exp = 0;

    NUM_QUOTES = 0;
    gen = nullptr;
}


