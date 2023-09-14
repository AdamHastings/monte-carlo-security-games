#include "Player.h"
#include <random>
#include <map>
#include <iostream>
#include <cassert>


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
static std::normal_distribution<double> p_defense_success_dist(0.388,0.062); // TODO justify these numbers.... BEFORE staring sims...


double discrete_wealth_dist(std::mt19937 generator) {
    std::discrete_distribution<> d({55, 33, 11, 1});
    int draw = d(generator);
    assert(draw >=0);
    assert(draw <= 3);
    if (draw == 0) {
        std::uniform_real_distribution<double> wealth_bucket_dist(0,9999);
        return wealth_bucket_dist(generator);
    } else if (draw == 1) {
        std::uniform_real_distribution<double> wealth_bucket_dist(10000,99999);
        return wealth_bucket_dist(generator);
    } else if (draw == 2) {
        std::uniform_real_distribution<double> wealth_bucket_dist(100000,999999);
        return wealth_bucket_dist(generator);
    } else if (draw == 3) {
        std::uniform_real_distribution<double> wealth_bucket_dist(1000000,9999999);
        return wealth_bucket_dist(generator);
    }
    std::cerr << "ERR: Never should see this" << std::endl;
    exit(1);
    return -1;
}

Defender::Defender(int id_in) : Player() {
    id = id_in;

    assets = discrete_wealth_dist(generator);
    if (assets < 0) {
        assets = 0;
    }

    probAttackSuccess = 1 - p_defense_success_dist(generator);
    if (probAttackSuccess < 0) {
        probAttackSuccess = 0;
    } else if (probAttackSuccess > 1) {
        probAttackSuccess = 1;
    }

    costToAttack = assets;
}

Attacker::Attacker(int id_in, double INEQUALITY) : Player() {
    id = id_in;

    assets = discrete_wealth_dist(generator) * INEQUALITY;
    if (assets < 0) {
        assets = 0;
    }
}