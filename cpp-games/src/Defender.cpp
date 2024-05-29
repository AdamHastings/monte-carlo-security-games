#include <algorithm>
#include <math.h>
#include <limits>
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
    lose(policy.premium);
    i->gain(policy.premium);
}

void Defender::submit_claim(uint32_t loss) {
    
    assert(insured); // you should only call this function if you have an active insurance policy
    assert(ins_idx >= 0); 
    // TODO: assert(ins_idx < alive_insurers.size());

    uint32_t claim_after_retention = std::max(0, ((int32_t)loss - (int32_t)policy.retention));
    assert(claim_after_retention >= 0);
    if (claim_after_retention > 0){
        if (insurers->at(ins_idx).is_alive()) {
            uint32_t amount_recovered = insurers->at(ins_idx).issue_payment(claim_after_retention);
            assert(loss >= amount_recovered);
            assert(claim_after_retention >= amount_recovered);
            assert(amount_recovered > 0);
            gain(amount_recovered);
        }
    }
}

void Defender::make_security_investment(uint32_t x) {
    defensesPurchased += 1;
    // uint32_t sec_investment_efficiency_draw = p.EFFICIENCY_distribution->draw();
    // posture = std::min(1.0, posture*(1 + sec_investment_efficiency_draw * (x / (assets*1.0))));
    assert(x >= 0);
    assert(x <= assets);
    double investment_pct = x / (double) assets;
    posture = posture_if_investment(investment_pct);
    assert(posture >= 0);
    assert(posture <= 1);
    sum_security_investments += x;
    lose(x);
    // TODO do we need to update any kind of costToAttack? Since it's been removed (for now)
    // costToAttack = assets * posture; // TODO this should be deprecated anyway
    // assert(costToAttack >= 0);
}

long long Defender::ransom(int assets) {
    return ransom_b0 + (assets * ransom_b1);
}

long long Defender::recovery_cost(int assets) {
    return recovery_base * pow(assets, recovery_exp);
}


double Defender::posture_if_investment(double investment_pct) {
    return erf(investment_pct * 25); // tODO remove 25,,,use config
}


// This can be improved by doing a gradient descent or binary search perhaps
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

// TODO this is only for one insurer...shouldn't Defender query all Insurers?
void Defender::choose_security_strategy() {

    assert(insurers->size() >= 0);

    Insurer* i = &insurers->at(0); // TODO iterate through all insurers!!!
    
    double p_A_hat = estimated_probability_of_attack;
    assert(p_A_hat >= 0);
    assert(p_A_hat <= 1);

    double p_L_hat = p_A_hat * (1 - posture);
    assert(p_L_hat >= 0);
    assert(p_L_hat <= 1);

    double mean_EFFICIENCY = p.EFFICIENCY_distribution->mean();
    assert(mean_EFFICIENCY >= 0);
    assert(mean_EFFICIENCY <= 1);

    // long long total_losses = ransom(assets) + recovery_cost(assets);

    // 1. Get insurance policy from insurer
    PolicyType policy = i->provide_a_quote(assets, 0); // TODO add noise to posture?
    
    bool insurable = true;
    long long expected_loss_with_insurance;
    if (policy.premium == 0 ||  policy.premium >= assets) {
        // Coverage not available
        insurable = false; 
    } else {
        assert(policy.premium > 0); 
        assert(policy.retention > 0);

        expected_loss_with_insurance = (uint32_t) policy.premium + (p_L_hat * policy.retention);
        assert(expected_loss_with_insurance >= 0);
    }


    // TODO maybe insurers *should* be able to write policies to defenders who have impenetrable...
    // it'd be like taking candy from a baby
    // but insurers need to still compute same loss ratio 
    // maybe insurers should keep a running total of their gains and losses
    // and only sell to super strong defenders if their loss ratios are too low.
    // this will change some things about the model though...will change underwriting quite a bit. 
    // but then what do they charge if they know they're ripping off the defender?
    // can they assume p_a_hat? 

    // 2. Find optimum security investment
    long long  optimal_investment = find_optimal_investment(); // TODO don't use PAYOFF anymore!! MUST RE-IMPLEMENT!! std::min(assets, std::max(0.0, (assets * (-1 + (assets * (mean_PAYOFF + posture * (-1 + mean_EFFICIENCY) * mean_PAYOFF))))/(2 * posture * p_A_hat * mean_EFFICIENCY * mean_PAYOFF))); 
    assert(optimal_investment <= assets);
    long long expected_cost_if_attacked_at_optimal_investment = ransom(assets - optimal_investment) + recovery_cost(assets - optimal_investment);
    assert(expected_cost_if_attacked_at_optimal_investment >= 0);
    double investment_pct = optimal_investment / (double) assets;
    double p_loss_with_optimal_investment = estimated_probability_of_attack * (1 -posture_if_investment(investment_pct));
    assert(p_loss_with_optimal_investment <= 1);
    assert(p_loss_with_optimal_investment >= 0);
    double expected_loss_with_optimal_investment = optimal_investment +  p_loss_with_optimal_investment * expected_cost_if_attacked_at_optimal_investment;
    assert(expected_loss_with_optimal_investment >= 0);
    // double expected_loss_with_optimal_investment = optimal_investment * p_L_hat; // TODO stand in for compilation DELETE !!!! TODO don't use PAYOFF anymore!! std::max(0.0, (assets - optimal_investment) * mean_PAYOFF * (p_A_hat * (1 - (posture * (mean_EFFICIENCY * (optimal_investment/assets))))) + optimal_investment);


    assert(optimal_investment >= 0);
    assert(expected_loss_with_optimal_investment >= 0);

    if (insurable && expected_loss_with_insurance < expected_loss_with_optimal_investment) {
        purchase_insurance_policy(i, policy);
    } else {
        make_security_investment(optimal_investment); // TODO what about case where optimal investment is greater than assets? 
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


