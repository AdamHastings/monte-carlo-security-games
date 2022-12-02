#include "Player.h"

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

int Defender::ctr = 0;

Defender::Defender() : Player() {
    id = ctr;
    ctr += 1;
}