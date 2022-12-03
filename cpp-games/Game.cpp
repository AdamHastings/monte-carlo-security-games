
#include <set>
#include <random>
#include <algorithm>
#include <iterator>
#include <cmath>
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

    last_delta_defenders_changes.assign(p.E, p.D);
    last_delta_attackers_changes.assign(p.E, p.D);

    if (p.verbose) {
        defenders_cumulative_assets.push_back(d_init);
        attackers_cumulative_assets.push_back(a_init);
        insurer_cumulative_assets.push_back(i_init);
        government_cumulative_assets.push_back(g_init);
    }

}

void Game::conclude_game(std::string outcome) {
    return; // TODO implement
}

bool Game::is_equilibrium_reached() {
    return false; // TODO
}

void Game::fight(Attacker a, Defender d) {
    return; // TODO implement
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

    for (int iter_num = 1; iter_num < p.N + 1; iter_num++) {

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
            Attacker a = attackers[i];
            Defender d = defenders[i];
            fight(a, d);

            if (std::round(a.assets) <= 0) {
                alive_attackers.erase(i);
            }
            if (std::round(d.assets) <= 0) {
                alive_defenders.erase(i);
            }
        }

        current_defender_sum_assets += defender_iter_sum;
        current_attacker_sum_assets += attacker_iter_sum;

        if (defenders_have_more_than_attackers) {
            if (current_attacker_sum_assets > current_defender_sum_assets) {
                crossovers.push_back(iter_num);
                defenders_have_more_than_attackers = false;
            }
        } else {
            if (current_attacker_sum_assets < current_defender_sum_assets) {
                crossovers.push_back(iter_num);
                defenders_have_more_than_attackers = true;
            }
        }

        if (p.verbose) {
            defenders_cumulative_assets.push_back(current_defender_sum_assets);
            attackers_cumulative_assets.push_back(current_attacker_sum_assets);
            insurer_cumulative_assets.push_back(insurer.assets);
            government_cumulative_assets.push_back(government.assets);
        }

        if (alive_attackers.size() == 0) {
            conclude_game("A");
            return;
        } else if (alive_defenders.size() == 0) {
            conclude_game("D");
            return;
        } else if (is_equilibrium_reached()) {
            conclude_game("E");
            return;
        }

    }

    conclude_game("N");
    return;
}