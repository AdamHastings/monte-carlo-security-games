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
}

double Defender::estimated_probability_of_attack = 0;

uint Defender::d_init = 0; 
double Defender::defender_iter_sum = 0;
double Defender::current_sum_assets = 0;

std::vector<double> Defender::cumulative_assets; 



void Defender::purchase_insurance_policy(Insurer &i, PolicyType p) {
    insured = true;
    ins_idx = i.id;
    policy = p;
    lose(p.premium);
    i.gain(p.premium);
}

void Defender::submit_claim(double loss) {
    
    assert(insured); // you should only call this function if you have an active insurance policy
    assert(ins_idx >= 0); 

    double claim_after_retention = (loss - policy.retention);
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

    Insurer i = insurers->at(0); // TODO iterate through all insurers!!!
    
    double p_A_hat = estimated_probability_of_attack;
    double p_L_hat = p_A_hat * (1 - posture);
    double mean_EFFICIENCY = p.EFFICIENCY_distribution->mean();
    double mean_PAYOFF = p.PAYOFF_distribution->mean();

    // 1. Get insurance policy from insurer
    std::cout << "costToAttackPercentile: " << costToAttackPercentile << std::endl; // TODO why is this nan?
    PolicyType policy = i.provide_a_quote(assets, posture, costToAttackPercentile); // TODO add noise to posture? or costToAttackPercentile?
    std::cout << policy.premium << " " << policy.retention << " " << p_L_hat << std::endl;
    double expected_loss_with_insurance = policy.premium +(p_L_hat * policy.retention);
    std::cout << "expected_loss_with_insurance: " << expected_loss_with_insurance << std::endl;
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
    double minimum = std::min({expected_loss_with_insurance, expected_loss_with_optimal_investment,expected_loss_with_perfect_security});
    if (minimum == expected_loss_with_insurance) {
        purchase_insurance_policy(i, policy);
    } else if (minimum == expected_loss_with_optimal_investment) {
        make_security_investment(optimal_investment);
    } else if (minimum == expected_loss_with_perfect_security) {
        make_security_investment(perfect_security_investment);
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


