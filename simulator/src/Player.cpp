#include "Player.h"
#include <random>
#include <map>
#include <iostream>
#include <algorithm>
#include <vector>
#include "Insurer.h"

Params Player::p;

Player::Player(Params &p_in) {
    assets = 0;
    p = p_in;
}

void Player::gain(int64_t gain) {
    assert(gain >= 0);
    assets += gain;
    assert(assets > 0); // Players do not exist in a state of long-term debt. 
}

void Player::lose(int64_t loss) {
    assert(loss >= 0);
    assert(loss <= assets);
    assets -= loss;
}

int64_t Player::get_assets() {
    return assets;
}

bool Player::is_alive() {
    return (assets > 0);
}

