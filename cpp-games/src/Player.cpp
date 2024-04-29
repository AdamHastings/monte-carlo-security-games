#include "Player.h"
#include <random>
#include <map>
#include <iostream>
#include <algorithm>
#include <vector>
#include "Insurer.h"



Player::Player(Params &p_in) {
    assets = 0;
    p = p_in;
}

void Player::gain(double gain) {
    assert(gain >= 0);
    assets += gain;
}

void Player::lose(double loss) {
    assert(loss >= 0);
    assert(loss <= assets);
    assets -= loss;
    if (assets == 0){
        alive = false;
    }
}

double Player::get_assets() {
    return assets;
}

