#include "Player.h"
#include <random>
#include <map>
#include <iostream>



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
    // Get insurance policy from insurer

    // find optimum security investment

    // decided which strategy is optimal.
}


Attacker::Attacker(int id_in, double INEQUALITY) : Player() {
    id = id_in;

    assets = wealth_dist(generator) * INEQUALITY;
    if (assets < 0) {
        assets = 0;
    }
}

PolicyType Insurer::provide_a_quote(double assets, double posture) {
    PolicyType policy;
    
    policy.premium = 0;
    policy.retention = 0;

    double OVerhead = 0.20; // 20% overhead
    double r = 20; // TODO double check retention regression factor

    double p_L = 0; // TODO calculate
    double u_P = 0; // TODO calculate
    
    policy.premium = (p_L * u_P * assets) / (r * p_L + OVerhead);
    policy.retention = r * policy.premium;


    return policy;
}