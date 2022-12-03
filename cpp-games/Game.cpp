
#include <set>
#include <random>
#include <algorithm>
#include <iterator>
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

void Game::fight(Attacker a, Defender d) {
    return;
}

void Game::run_iterations() {

    std::set<int> alive_attackers;
    for (int i=0; i<attackers.size(); i++) {
        alive_attackers.insert(i);
    }

    std::set<int> alive_defenders;
    for (int i=0; i<defenders.size(); i++) {
        alive_defenders.insert(i);
    }

    bool defenders_have_more_than_attackers = true;

    for (int iter_num = 0; iter_num < p.N; iter_num++) {

        float defender_iter_sum = 0, attacker_iter_sum = 0;

        std::vector<int> alive_attackers_list(alive_attackers.begin(), alive_attackers.end()); // TODO maybe optimize this later
        std::vector<int> alive_defenders_list(alive_defenders.begin(), alive_defenders.end()); // TODO maybe optimize this later

        std::random_device rd;
        std::mt19937 g(rd());

        int shorter_length;
        if (alive_defenders_list.size() < alive_attackers_list.size()) {
            std::shuffle(alive_attackers_list.begin(), alive_attackers_list.end(), g);
            shorter_length = alive_defenders_list.size();
        } else {
            std::shuffle(alive_attackers_list.begin(), alive_attackers_list.end(), g);
            shorter_length = alive_attackers_list.size();
        }

        for (int i=0; i<shorter_length; i++) {
            fight(attackers[i], defenders[i]);
        }

    }
    

}