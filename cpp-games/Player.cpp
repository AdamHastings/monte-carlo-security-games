#include "Player.h"
#include <random>


Player::Player() {
    assets = 0;
}

void Player::gain(float gain) {
    assets += gain;
}

void Player::lose(float loss) {
    assets -= loss;
}

float Player::get_assets() {
    return assets;
}

int Defender::s_ctr = 0;

static std::default_random_engine generator;
static std::lognormal_distribution<float> wealth(10.0, 1.0);
static std::normal_distribution<float> p_defense_success(0.388,0.062);

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

int Attacker::s_ctr = 0;

Attacker::Attacker(float INEQUALITY) : Player() {
    id = s_ctr;
    s_ctr += 1;
    
    std::default_random_engine generator;

    std::lognormal_distribution<float> lognormal(10.0, 1.0);
    assets = lognormal(generator) * INEQUALITY;

}