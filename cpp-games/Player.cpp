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

Defender::Defender() : Player() {
    id = s_ctr;
    s_ctr += 1;

    std::default_random_engine generator;

    std::lognormal_distribution<float> lognormal(10.0, 1.0);
    assets = lognormal(generator);

    std::normal_distribution<double> normal(0.388,0.062);
    probAttackSuccess = 1 - normal(generator);
    if (probAttackSuccess < 0) {
        probAttackSuccess = 0;
    } else if (probAttackSuccess > 1) {
        probAttackSuccess = 1;
    }

    costToAttack = assets;
}

int Attacker::s_ctr = 0;

Attacker::Attacker() : Player() {
    id = s_ctr;
    s_ctr += 1;
    
    std::default_random_engine generator;

    std::lognormal_distribution<float> lognormal(10.0, 1.0);
    assets = lognormal(generator); // This is scaled by INEQUALITY later

}