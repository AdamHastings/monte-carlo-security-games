#include "Player.h"
#include <random>
#include <map>
#include <iostream>
#include <algorithm>



Player::Player() {
    assets = 0;
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

// static std::random_device rd;  // Will be used to obtain a seed for the random number engine
// static std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
static std::mt19937 generator(0);
// static std::default_random_engine generator;
static std::lognormal_distribution<double> wealth_dist(9.875392795, 1.682545179);
static std::normal_distribution<double> posture_dist(0.388,0.062); // TODO justify these numbers.... BEFORE staring sims...

Defender::Defender(int id_in) : Player() {
    id = id_in;

    assets = wealth_dist(generator);
    if (assets < 0) {
        assets = 0;
    }

    posture = posture_dist(generator);
    if (posture < 0) {
        posture = 0;
    } else if (posture > 1) {
        posture = 1;
    }

    costToAttack = assets * posture;
}

void Defender::make_investment() {

    double p_L_hat = 0; // TODO Defender's best guess estimate of the probability of being attacked
    double u_E = 0; // TODO fill in later
    double u_P = 0; // TODO fill in later


    // 1. Get insurance policy from insurer
    PolicyType policy = Insurer::provide_a_quote(assets, posture); // 
    double expected_loss_with_insurance = policy.premium +(p_L_hat * policy.retention);

    // 2. Find optimum security investment
    double optimal_investment = 0; // TODO
    double expected_loss_with_optimal_investment = p_L_hat*(assets - optimal_investment) * u_P - optimal_investment;

    // 3. Find cost to achieve perfect security
    double expected_loss_with_perfect_security = (assets * (1 - posture))/ (u_E * posture);

    // Choose the optimal strategy.
    double minimum = std::min({expected_loss_with_insurance, expected_loss_with_optimal_investment,expected_loss_with_perfect_security});
    if (minimum == expected_loss_with_insurance) {
        // buy insurnace
        return;
    }

    return;

    // decided which strategy is optimal.
}


Attacker::Attacker(int id_in, double INEQUALITY) : Player() {
    id = id_in;

    assets = wealth_dist(generator) * INEQUALITY;
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