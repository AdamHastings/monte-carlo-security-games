#include "Player.h"
#include <random>
#include <map>


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

int Defender::s_ctr = 0;

static std::default_random_engine generator;
static std::lognormal_distribution<double> wealth(10.0, 1.0);
static std::normal_distribution<double> p_defense_success(0.388,0.062);

Defender::Defender() : Player() {
    id = s_ctr;
    s_ctr += 1;

    assets = wealth(generator);

    probAttackSuccess = 1 - p_defense_success(generator);
    if (probAttackSuccess < 0) {
        probAttackSuccess = 0;
    } else if (probAttackSuccess > 1) {
        probAttackSuccess = 1;
    }

    costToAttack = assets;
}

void Defender::reset_ctr() {
    s_ctr = 0;
}

int Attacker::s_ctr = 0;

Attacker::Attacker(double INEQUALITY) : Player() {
    id = s_ctr;
    s_ctr += 1;
    
    assets = wealth(generator) * INEQUALITY;
}

void Attacker::reset_ctr() {
    s_ctr = 0;
}