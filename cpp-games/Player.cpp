#include "Player.h"
#include <random>
#include <map>
#include <iostream>
#include <algorithm>



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

void Defender::purchase_insurance_policy(Insurer &i, PolicyType p) {
    insured = true;
    lose(p.premium);
    i.gain(p.premium);
}

void Defender::make_security_investment(double x) {
    double sec_investment_efficiency_draw = p.EFFICIENCY_distribution->draw();
    posture = std::min(1.0, posture*(1 + sec_investment_efficiency_draw * (x / assets)));
    lose(x);
}

void Defender::choose_security_strategy(Insurer i) {

    double p_A_hat = 0; // TODO get somehow
    double p_L_hat = p_A_hat * (1 - posture);
    double mean_EFFICIENCY = p.EFFICIENCY_distribution->mean();
    double mean_PAYOFF = p.PAYOFF_distribution->mean();


    // 1. Get insurance policy from insurer
    PolicyType policy = Insurer::provide_a_quote(assets, posture); // TODO add noise to posture?
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

PolicyType Insurer::provide_a_quote(double assets, double estimated_posture) {
    PolicyType policy;

    double OVerhead = 0.20; // 20% overhead
    double r = 20; // TODO double check retention regression factor

    double p_A = 0; // TODO calculate
    double p_L = p_A * (1 - estimated_posture); 
    double u_P = 0; // TODO calculate
    
    policy.premium = (p_L * u_P * assets) / (r * p_L + OVerhead);
    policy.retention = r * policy.premium;


    return policy;
}