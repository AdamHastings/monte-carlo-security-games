#include <algorithm>
#include <math.h>
#include "Defender.h"

double Defender::estimated_probability_of_attack = 0;
double Defender::d_init = 0; 
double Defender::defender_iter_sum = 0;
double Defender::current_sum_assets = 0;
double Defender::sum_recovery_costs = 0;
unsigned int Defender::policiesPurchased = 0;
unsigned int Defender::defensesPurchased = 0;
std::vector<double> Defender::cumulative_assets;

Defender::Defender(int id_in, Params &p, std::vector<Insurer>& _insurers) : Player(p) {
    id = id_in;

    insurers = &_insurers;

    // parameters scaled down by 1B during curve fitting to avoid numerical overflow
    // so I re-scale back up by 1B here to compensate
    assets = p.WEALTH_distribution->draw() * pow(10, 9); 
    if (assets < 0) {
        assets = 0;
    }

    posture = p.POSTURE_distribution->draw();
    if (posture < 0) {
        posture = 0;
    } else if (posture > 1) {
        posture = 1;
    }

    costToAttack = assets * posture;

    d_init += assets;
    current_sum_assets += assets;

    assert(Defender::d_init == Defender::current_sum_assets);
} 

// TODO what if retention > assets????
void Defender::purchase_insurance_policy(Insurer* i, PolicyType p) {
    policiesPurchased += 1;
    
    assert(assets > p.premium);
    
    insured = true;
    ins_idx = i->id;
    policy = p;
    lose(policy.premium);

    i->gain(policy.premium);

    assert(assets > p.retention); // TODO not sure about this...based on odds, some defenders may YOLO 
    // TODO maybe we should only sell policies if Defenders are able to pay the retention
    // Or maybe if retention > assets, there's no point in buying insurance so they automatically buy security? TODO TODO TODO
}

void Defender::submit_claim(double loss) {
    
    assert(insured); // you should only call this function if you have an active insurance policy
    assert(ins_idx >= 0); 

    double claim_after_retention = std::max(0.0, (loss - policy.retention));
    assert(claim_after_retention >= 0);
    if (claim_after_retention > 0) {
        if (insurers->at(ins_idx).alive) {
            double amount_recovered = insurers->at(ins_idx).issue_payment(claim_after_retention);
            assert(loss >= amount_recovered);
            assert(claim_after_retention >= amount_recovered);
            assert(amount_recovered > 0);
            gain(amount_recovered);
        }
    }
}

void Defender::make_security_investment(double x) {
    defensesPurchased += 1;
    double sec_investment_efficiency_draw = p.EFFICIENCY_distribution->draw();
    posture = std::min(1.0, posture*(1 + sec_investment_efficiency_draw * (x / (assets*1.0))));
    assert(posture >= 0);
    assert(posture <= 1);
    lose(x);
    costToAttack = assets * posture; // Why wasn't this included until now? // TODO this should be deprecated anyway
    assert(costToAttack >= 0);
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

    double ransom = p.RANSOM_BASE_distribution->mean() * pow(assets,  p.RANSOM_EXP_distribution->mean());
    double recovery_cost = p.RECOVERY_COST_BASE_distribution->mean() * pow(assets, p.RECOVERY_COST_EXP_distribution->mean());
    double total_losses = ransom + recovery_cost;

    // 1. Get insurance policy from insurer
    PolicyType policy = i->provide_a_quote(assets, posture, costToAttackPercentile); // TODO add noise to posture? or costToAttackPercentile?
    double expected_loss_with_insurance = policy.premium +(p_L_hat * policy.retention);
    assert(policy.premium > 0); // I'd like to not have to consider cases where premium = 0
    assert(policy.retention > 0);
    assert(expected_loss_with_insurance >= 0);
    // TODO maybe insurers *should* be able to write policies to defenders who have impenetrable...
    // it'd be like taking candy from a baby
    // but insurers need to still compute same loss ratio 
    // maybe insurers should keep a running total of their gains and losses
    // and only sell to super strong defenders if their loss ratios are too low.
    // this will change some things about the model though...will change underwriting quite a bit. 
    // but then what do they charge if they know they're ripping off the defender?
    // can they assume p_a_hat? 

    // 2. Find optimum security investment
    double optimal_investment = 0; // TODO don't use PAYOFF anymore!! MUST RE-IMPLEMENT!! std::min(assets, std::max(0.0, (assets * (-1 + (assets * (mean_PAYOFF + posture * (-1 + mean_EFFICIENCY) * mean_PAYOFF))))/(2 * posture * p_A_hat * mean_EFFICIENCY * mean_PAYOFF)));
    double expected_loss_with_optimal_investment = total_losses; // TODO stand in for compilation DELETE !!!! TODO don't use PAYOFF anymore!! std::max(0.0, (assets - optimal_investment) * mean_PAYOFF * (p_A_hat * (1 - (posture * (mean_EFFICIENCY * (optimal_investment/assets))))) + optimal_investment);
    assert(optimal_investment >= 0);
    assert(expected_loss_with_optimal_investment >= 0);

    // Choose the optimal strategy.
    if (expected_loss_with_insurance < expected_loss_with_optimal_investment) {
        purchase_insurance_policy(i, policy);
    } else {
        make_security_investment(optimal_investment);
    }
}


void Defender::lose(double loss) {
    Player::lose(loss);
    defender_iter_sum -= loss;
    current_sum_assets -= loss;
}

void Defender::gain(double gain) {
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
    policiesPurchased = 0;
    defensesPurchased = 0;
    sum_recovery_costs = 0;
    cumulative_assets.clear();
}


