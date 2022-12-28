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
static std::lognormal_distribution<double> wealth(9.875392795, 1.682545179);
static std::normal_distribution<double> p_defense_success(0.388,0.062); // TODO justify these numbers.... BEFORE staring sims...

Defender::Defender(int id_in) : Player() {
    id = id_in;

    assets = wealth(generator);
    if (assets < 0) {
        assets = 0;
    }

    probAttackSuccess = 1 - p_defense_success(generator);
    if (probAttackSuccess < 0) {
        probAttackSuccess = 0;
    } else if (probAttackSuccess > 1) {
        probAttackSuccess = 1;
    }

    costToAttack = assets;
}

Attacker::Attacker(int id_in, double INEQUALITY) : Player() {
    id = id_in;

    assets = wealth(generator) * INEQUALITY;
    if (assets < 0) {
        assets = 0;
    }
}