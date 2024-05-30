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

    posture = p.POSTURE_distribution->draw();
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
    i->gain(policy.premium); // TODO maybe but into Insurer.cpp
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

void Defender::make_security_investment(uint32_t x) {
    defensesPurchased += 1;
    assert(x >= 0);
    assert(x <= assets);
    double investment_pct = x / (double) assets;
    posture = posture_if_investment(investment_pct); // TODO try out alternative definitions
    assert(posture >= 0);
    assert(posture <= 1);
    sum_security_investments += x;
    this->lose(x);
}

long long Defender::ransom(int assets) {
    return ransom_b0 + (assets * ransom_b1);
}

long long Defender::recovery_cost(int assets) {
    return recovery_base * pow(assets, recovery_exp);
}


double Defender::posture_if_investment(double investment_pct) {
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
        double p_loss = Defender::estimated_probability_of_attack * (1 - posture_if_investment(inv_percent)); 
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

// TODO this is too harsh of depreciation 
void Defender::security_depreciation() {
    posture = 0;
}

void Defender::choose_security_strategy() {

    double p_A_hat = estimated_probability_of_attack;
    assert(p_A_hat >= 0);
    assert(p_A_hat <= 1);

    double p_L_hat = p_A_hat * (1 - posture);
    assert(p_L_hat >= 0);
    assert(p_L_hat <= 1);

    // double mean_EFFICIENCY = p.EFFICIENCY_distribution->mean();
    // assert(mean_EFFICIENCY >= 0);
    // assert(mean_EFFICIENCY <= 1);

    // 1. Get insurance policy from insurer
    uint32_t num_quotes_requested = p.NUM_QUOTES_distribution->draw();
    std::uniform_int_distribution<> insurer_indices_dist(0, insurers->size()-1);
    // pick insurers for quotes
    // TODO think about how to do this in a cache-friendly way
    std::unordered_set<unsigned int> insurer_indices;
    while (insurer_indices.size() < num_quotes_requested) {
        // TODO consider having attackers fight until they've attempted ATTACKS_PER_EPOCH attacks
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

        // TODO assumes that defender maintains posture after policy is purchased, which currently is untrue.
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
    assert(optimal_investment <= assets);
    int64_t expected_cost_if_attacked_at_optimal_investment = ransom(assets - optimal_investment) + recovery_cost(assets - optimal_investment);
    assert(expected_cost_if_attacked_at_optimal_investment >= 0);
    double investment_pct = optimal_investment / (double) assets;
    double p_loss_with_optimal_investment = estimated_probability_of_attack * (1 -posture_if_investment(investment_pct));
    assert(p_loss_with_optimal_investment <= 1);
    assert(p_loss_with_optimal_investment >= 0);
    double expected_loss_with_optimal_investment = optimal_investment +  p_loss_with_optimal_investment * expected_cost_if_attacked_at_optimal_investment;
    assert(expected_loss_with_optimal_investment >= 0);

    assert(optimal_investment >= 0);
    assert(expected_loss_with_optimal_investment >= 0);

    if (insurable && expected_loss_with_insurance < expected_loss_with_optimal_investment) {
        purchase_insurance_policy(best_insurer, best_policy);
        security_depreciation(); // TODO should this happen in both cases? i.e. even if we make_security_investment
    } else {
        make_security_investment(optimal_investment); // TODO what about case where optimal investment is greater than assets? 
    }
}

// TODO consider the possibility of multiple attacks
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

// TODO name conflict confusion with reset_alive_players in Game.cpp
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


