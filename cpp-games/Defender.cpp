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

}

double Defender::estimated_probability_of_attack = 0;

double Defender::d_init = 0; 
double Defender::defender_iter_sum = 0;
double Defender::current_sum_assets = 0;

std::vector<double> Defender::cumulative_assets; 


// TODO what if retention > assets????
void Defender::purchase_insurance_policy(Insurer* i, PolicyType p) {
    assert(assets > p.premium);
    
    insured = true;
    ins_idx = i->id;
    policy = p;
    lose(policy.premium);
    // std::cout << "      Insurer 0 has (before selling policy) " << i->assets << std::endl;


    i->gain(policy.premium);
    // std::cout << "      Insurer 0 has (after selling policy) " << i->assets << std::endl;


    assert(assets > p.retention); // TODO not sure about this...based on odds, some defenders may YOLO 
    // TODO maybe we should only sell policies if Defenders are able to pay the retention
    // Or maybe if retention > assets, there's no point in buying insurance so they automatically buy security? TODO TODO TODO

    // std::cout << "     Defender " << id << " now has " << assets << std::endl;

}

void Defender::submit_claim(double loss) {
    
    assert(insured); // you should only call this function if you have an active insurance policy
    assert(ins_idx >= 0); 

    double claim_after_retention = std::max(0.0, (loss - policy.retention));
    assert(claim_after_retention > 0);
    if (claim_after_retention > 0) {
        double amount_recovered = insurers->at(ins_idx).issue_payment(claim_after_retention);
        assert(loss >= amount_recovered);
        assert(claim_after_retention >= amount_recovered);
        assert(amount_recovered > 0);
        lose(claim_after_retention - amount_recovered);
    }
}

// TODO this is why the checksums are failing...transactions not known to Game class
void Defender::make_security_investment(double x) {
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
    double p_L_hat = p_A_hat * (1 - posture);
    double mean_EFFICIENCY = p.EFFICIENCY_distribution->mean();
    double mean_PAYOFF = p.PAYOFF_distribution->mean();

    // 1. Get insurance policy from insurer
    PolicyType policy = i->provide_a_quote(assets, posture, costToAttackPercentile); // TODO add noise to posture? or costToAttackPercentile?
    double expected_loss_with_insurance = policy.premium +(p_L_hat * policy.retention);
    assert(policy.premium > 0);
    assert(policy.retention > 0);
    assert(expected_loss_with_insurance >= 0);

    // 2. Find optimum security investment
    double optimal_investment = std::min(assets, std::max(0.0, (assets * (-1 + (assets * (mean_PAYOFF + posture * (-1 + mean_EFFICIENCY) * mean_PAYOFF))))/(2 * posture * p_A_hat * mean_EFFICIENCY * mean_PAYOFF)));
    double expected_loss_with_optimal_investment = std::max(0.0, (assets - optimal_investment) * mean_PAYOFF * (p_A_hat * (1 - (posture * (mean_EFFICIENCY * (optimal_investment/assets))))) + optimal_investment);
    assert(optimal_investment >= 0);
    assert(expected_loss_with_optimal_investment >= 0);


    // 3. Find cost to achieve perfect security
    double perfect_security_investment = (assets * (assets * posture)) / (posture * mean_EFFICIENCY);
    double expected_loss_with_perfect_security = perfect_security_investment;
    assert(perfect_security_investment >= 0);
    assert(expected_loss_with_perfect_security >= 0);

    // Choose the optimal strategy.
    // TODO premiums are a bit high!! look into this. maybe set the #attackers to be such that intial premiums match existing payments
    double minimum = std::min({expected_loss_with_insurance, expected_loss_with_optimal_investment,expected_loss_with_perfect_security});
    if (minimum == expected_loss_with_insurance) {
        // std::cout << "     Defender " << id << " with assets=" << assets << " is purchasing insurance with premium=" << policy.premium << " and retention=" << policy.retention << std::endl;
        purchase_insurance_policy(i, policy);
        // std::cout << "     Defender " << id << " now has " << assets << std::endl;
    } else if (minimum == expected_loss_with_optimal_investment) {
        // std::cout << "     Defender " << id << " is investing in security" << std::endl;
        make_security_investment(optimal_investment);
    } else if (minimum == expected_loss_with_perfect_security) {
        // std::cout << "     Defender " << id << " is achieving perfect security!" << std::endl;
        make_security_investment(perfect_security_investment);
    } else {
        assert(false); // should never reach this
    }
}


void Defender::lose(double loss) {
    // std::cout << "       Defender " << id << " losing " << loss << std::endl;

    Player::lose(loss);
    defender_iter_sum -= loss;
    current_sum_assets -= loss;
}

void Defender::gain(double gain) {
    Player::gain(gain);
    defender_iter_sum += gain;
    current_sum_assets += gain;
}


