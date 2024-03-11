#include "Player.h"
#include <random>
#include <map>
#include <iostream>
#include <algorithm>
#include <vector>



Player::Player(Params &p_in) {
    assets = 0;
    p = p_in;
}

void Player::gain(double gain) {
    assets += gain;
}

void Player::lose(double loss) {
    assets -= loss;
}

double Player::get_assets() {
    return assets;
}

Insurer::Insurer(int id_in, Params &p) : Player(p) {
    id = id_in;
}

// Should be set each iteration by the game. 
int Insurer::num_defenders = 0;
int Insurer::num_attackers = 0;
std::vector<double> Insurer::attacker_assets;

PolicyType Insurer::provide_a_quote(double assets, double estimated_posture, double estimated_costToAttackPercentile) {    
    PolicyType policy;

    double OVerhead = 0.20; // 20% overhead // Better to call this a "loss ratio" actually (standard terminology for insurance)
    double r = 20.0; // TODO double check retention regression factor



    double probability_of_getting_paried_with_attacker = std::min(1.0, ((num_attackers * 1.0) / (num_defenders * 1.0)));
    double probability_random_attacker_has_enough_to_attack = (1 - estimated_costToAttackPercentile);
    double p_A = probability_of_getting_paried_with_attacker * probability_random_attacker_has_enough_to_attack;
    double p_L = p_A * (1 - estimated_posture); 
    double mean_PAYOFF = p.PAYOFF_distribution->mean();
    
    policy.premium = (p_L * mean_PAYOFF * assets) / (r * p_L + OVerhead);
    policy.retention = r * policy.premium;


    return policy;
}

Defender::Defender(int id_in, Params &p) : Player(p) {
    id = id_in;

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
}

double Defender::estimated_probability_of_attack = 0;

void Defender::purchase_insurance_policy(Insurer &i, PolicyType p) {
    insured = true;
    lose(p.premium);
    i.gain(p.premium);
}

void Defender::make_security_investment(double x) {
    double sec_investment_efficiency_draw = p.EFFICIENCY_distribution->draw();
    posture = std::min(1.0, posture*(1 + sec_investment_efficiency_draw * (x / (assets*1.0))));
    assert(posture >= 0);
    assert(posture <= 1);

    lose(x);
}

// TODO this is only for one insurer...shouldn't Defender query all Insurers?
void Defender::choose_security_strategy(Insurer &i) {

    double p_A_hat = estimated_probability_of_attack;
    double p_L_hat = p_A_hat * (1 - posture);
    double mean_EFFICIENCY = p.EFFICIENCY_distribution->mean();
    double mean_PAYOFF = p.PAYOFF_distribution->mean();

    // 1. Get insurance policy from insurer
    PolicyType policy = i.provide_a_quote(assets, posture, costToAttackPercentile); // TODO add noise to posture? or costToAttackPercentile?
    double expected_loss_with_insurance = policy.premium +(p_L_hat * policy.retention);
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


Attacker::Attacker(int id_in, Params &p) : Player(p) {
    id = id_in;

    assets = p.WEALTH_distribution->draw() * p.INEQUALITY;
    if (assets < 0) {
        assets = 0;
    }
}