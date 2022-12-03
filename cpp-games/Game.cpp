
#include "Game.h"

Game::Game(Params p, std::vector<Defender> d, std::vector<Attacker> a, Insurer i, Government g) {
    p = p;
    defenders = d;
    attackers = a;
    insurer = i;
    government = g;

    for (auto di : defenders) {
        d_init += di.get_assets();
    }

    for (auto ai : attackers) {
        a_init += ai.get_assets();
    }

    i_init = insurer.get_assets();
    g_init = insurer.get_assets();

    // TODO likely more that needs to happen here
}

void Game::run_iterations() {

}