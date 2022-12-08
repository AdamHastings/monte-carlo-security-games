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

static std::default_random_engine generator;
static std::lognormal_distribution<double> wealth(10.0, 1.0);
static std::normal_distribution<double> p_defense_success(0.388,0.062);

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
    int id = id_in;

    assets = wealth(generator) * INEQUALITY;
    if (assets < 0) {
        assets = 0;
    }

    std::cout << id << " ,";

}