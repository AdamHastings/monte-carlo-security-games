#include <algorithm>
#include "Defender.h"

Defender::Defender(int id_in, Params &p, std::vector<Insurer>& _insurers) : Player(p) {
    id = id_in;

    insurers = &_insurers;

    assets = p.WEALTH_distribution->draw();
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

double Defender::estimated_probability_of_attack = 0;

double Defender::d_init = 0; 
double Defender::defender_iter_sum = 0;
double Defender::current_sum_assets = 0;

unsigned int Defender::policiesPurchased = 0;
unsigned int Defender::defensesPurchased = 0;

std::vector<double> Defender::cumulative_assets; 


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

// TODO this is why the checksums are failing...transactions not known to Game class
void Defender::make_security_investment(double x) {
    defensesPurchased += 1;
    double sec_investment_efficiency_draw = p.EFFICIENCY_distribution->draw();
    posture = std::min(1.0, posture*(1 + sec_investment_efficiency_draw * (x / (assets*1.0))));
    assert(posture >= 0);
    assert(posture <= 1);
    lose(x);
    costToAttack = assets * posture; // Why wasn't this included until now?
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

    double mean_PAYOFF = p.PAYOFF_distribution->mean();
    assert(mean_PAYOFF >= 0);
    assert(mean_PAYOFF <= 1);

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
    double optimal_investment = std::min(assets, std::max(0.0, (assets * (-1 + (assets * (mean_PAYOFF + posture * (-1 + mean_EFFICIENCY) * mean_PAYOFF))))/(2 * posture * p_A_hat * mean_EFFICIENCY * mean_PAYOFF)));
    double expected_loss_with_optimal_investment = std::max(0.0, (assets - optimal_investment) * mean_PAYOFF * (p_A_hat * (1 - (posture * (mean_EFFICIENCY * (optimal_investment/assets))))) + optimal_investment);
    assert(optimal_investment >= 0);
    assert(expected_loss_with_optimal_investment >= 0);


    // 3. Find cost to achieve perfect security
    // TODO shouldn't this never happen?
    // double perfect_security_investment = (assets * (assets * posture)) / (posture * mean_EFFICIENCY);
    // double expected_loss_with_perfect_security = perfect_security_investment;
    // assert(perfect_security_investment >= 0);
    // assert(expected_loss_with_perfect_security >= 0);

    // Choose the optimal strategy.
    // TODO premiums are a bit high!! look into this. maybe set the #attackers to be such that intial premiums match existing payments
    // TODO think about what happens when premiums are 0. Does this count as a valid policy? Does this mean expected loss is 0? What happens then?
    // double minimum = std::min({expected_loss_with_insurance, expected_loss_with_optimal_investment,expected_loss_with_perfect_security});
    double minimum = std::min({expected_loss_with_insurance, expected_loss_with_optimal_investment});

    if (minimum == expected_loss_with_insurance) {
        purchase_insurance_policy(i, policy);
    } else if (minimum == expected_loss_with_optimal_investment) {
        make_security_investment(optimal_investment);
    // } else if (minimum == expected_loss_with_perfect_security) {
    //     make_security_investment(perfect_security_investment);
    } else {
        assert(false); // should never reach this
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

void Defender::reset() {
    estimated_probability_of_attack = 0;
    d_init = 0;
    defender_iter_sum = 0;
    current_sum_assets = 0; 
    policiesPurchased = 0;
    defensesPurchased = 0;
    cumulative_assets.clear();
}


