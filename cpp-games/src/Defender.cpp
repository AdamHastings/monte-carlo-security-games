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

    // posture = p.POSTURE_distribution->draw();
    capex = (int64_t) fp_assets * 0.01; // initialize defenders with initial capex that will yield average posture // TODO make this an input param?
    double noise = p.POSTURE_NOISE_distribution->draw();
    posture = posture_if_investment(capex) + noise;


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
    policiesPurchased += 1;
    assert(assets > p.premium);
    insured = true;
    ins_idx = i->id;
    policy = p;
    this->lose(policy.premium);
    i->gain(policy.premium); // May be worth considering putting this into Insurer.cpp
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
    defensesPurchased += 1;
    assert(amount >= 0);
    assert(amount <= assets);
    posture = posture_if_investment(amount);
    assert(posture >= 0);
    assert(posture <= 1);
    sum_security_investments += amount;
    this->lose(amount);

    capex += amount / 3; // opex is twice capex spending ==> amount = capex + opex = capex + 2 * capex ==> (new) capex = amount / 3
}

long long Defender::ransom(int assets) {
    return ransom_b0 + (assets * ransom_b1);
}

long long Defender::recovery_cost(int assets) {
    return recovery_base * pow(assets, recovery_exp);
}

// yields the expected posture if a defender were to invest amount into security
double Defender::posture_if_investment(int64_t amount) {
    double investment_pct = (double) amount / (double) assets;
    return erf(investment_pct * 25); // TODO remove 25, use config instead 
}


// TODO This can be approximated using Newton Raphon method
double Defender::find_optimal_investment(){
    int samples = 1000; // sample at 1% increments
    double minimum = std::numeric_limits<double>::max(); // TODO use inf instead?
    double optimal_investment = 0;
    double last_round_loss= -std::numeric_limits<double>::max();
    for (int i=0; i<samples; i++) {
        double inv_percent = ((double) i/ (double) samples);
        long long investment = (long long) assets * inv_percent;
        double p_loss = Defender::estimated_probability_of_attack * (1 - posture_if_investment(investment)); 
        long long cost_if_attacked = ransom(assets - investment) + recovery_cost(assets - investment);

        double loss = investment + p_loss * cost_if_attacked;

        if (loss < minimum) {
            minimum = loss;
            optimal_investment = investment;
        } else if (loss > last_round_loss) { // This function was killing performance...hopefully this reduces calculations
            break;
        }
        last_round_loss = loss;

        
    } // TODO add assertions 
    return optimal_investment; // TODO return struct with minimum *and* minimum_pct
}

void Defender::security_depreciation() {
    // We assume that opex is twice capex
    // the value of previous opex spending depreciates to zero after it is spent (by definition)
    double DEPRECIATION = 0.4; // TODO put into config // justify this value
    capex = capex * (1 - DEPRECIATION);
    posture = posture_if_investment(capex);
}

void Defender::choose_security_strategy() {

    double p_A_hat = estimated_probability_of_attack;
    assert(p_A_hat >= 0);
    assert(p_A_hat <= 1);

    double p_L_hat = p_A_hat * (1 - posture);
    assert(p_L_hat >= 0);
    assert(p_L_hat <= 1);

    // 1. Get insurance policy from insurer
    std::uniform_int_distribution<> insurer_indices_dist(0, insurers->size()-1);
    
    // pick insurers for quotes
    // TODO think about how to do this in a cache-friendly way
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
    if (insurable && expected_loss_with_insurance < expected_loss_with_optimal_investment) {
        purchase_insurance_policy(best_insurer, best_policy);
    } else {
        make_security_investment(optimal_investment);
    }
}

void Defender::perform_market_analysis(int prevRoundAttacks, int num_current_defenders) {
    // Defenders don't have the same visibility as the insurers but still can make some predictions about risk.
    assert(prevRoundAttacks <= num_current_defenders);
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
}


