
#include <set>
#include <random>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <ctime>
#include <iostream>
#include "Game.h"


static std::random_device rd;  // Will be used to obtain a seed for the random number engine
static std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
std::uniform_real_distribution<> uniform(0.0, 1.0);

Game::Game(Params prm, std::vector<Defender> d, std::vector<Attacker> a, Insurer i, Government g) {
    p = prm;
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

    for (int i=0; i<attackers.size(); i++) {
        alive_attackers.insert(i);
    }
    
    for (int i=0; i<defenders.size(); i++) {
        alive_defenders.insert(i);
    }

    last_delta_defenders_changes.assign(p.E, p.D);
    last_delta_attackers_changes.assign(p.E, p.D);

    if (p.verbose) {
        defenders_cumulative_assets.push_back(d_init);
        attackers_cumulative_assets.push_back(a_init);
        insurer_cumulative_assets.push_back(i_init);
        government_cumulative_assets.push_back(g_init);
    }

}

void Game::verify_state() {
    return;
}

void Game::conclude_game(std::string outcome) {
    final_outcome = outcome;
    verify_state();
}

bool Game::is_equilibrium_reached() {
    return false; // TODO
}

void Game::a_steals_from_d(Attacker a, Defender d, float loot) {
    d_gain(d, loot);
    a_gain(a, loot);
    attackerLoots += loot;

    // Check if this d has previously been attacked by a
    if (a.victims.find(d.id) != a.victims.end()) {
        a.victims[d.id] += loot;
    } else {
        a.victims[d.id] = loot;
    }
}

void Game::d_gain(Defender d, float gain) {
    d.gain(gain);
    defender_iter_sum += gain;
}

void Game::d_lose(Defender d, float loss) {
    d.lose(loss);
    defender_iter_sum -= loss;
}

void Game::a_gain(Attacker a, float gain) {
    a.gain(gain);
    attacker_iter_sum += gain;
}

void Game::a_lose(Attacker a, float loss) {
    a.lose(loss);
    attacker_iter_sum -= loss;
}

void Game::d_recoup(Attacker a, Defender d, float recoup_amount) {
    if (d.claimsReceived.find(a.id) != d.claimsReceived.end()) {
        // Split the recoup amount between the defender and the Insurer

        if (recoup_amount >= d.claimsReceived[a.id]) {
            d_gain(d, recoup_amount - d.claimsReceived[a.id]);
            insurer_recoup(d.claimsReceived[a.id]);
            d.claimsReceived[a.id] = 0;
            alive_defenders.insert(d.id);
        } else {
            insurer_recoup(recoup_amount);
            d.claimsReceived[a.id] -= recoup_amount;
        }
    } else {
        alive_defenders.insert(d.id);
        d_gain(d, recoup_amount);
    }
}

void Game::insurer_lose(float loss) {
    insurer.lose(loss);
    paidClaims += loss;
}

void Game::insurer_covers_d_for_losses_from_a(Attacker a, Defender d, float claim) {
    float claims_amount = claim * p.CLAIMS;
    if (claims_amount > insurer.assets) {
        claims_amount = insurer.assets;
        insurerTimesOfDeath.push_back(iter_num);
    }

    d_gain(d, claims_amount);
    if (d.claimsReceived.find(a.id) != d.claimsReceived.end()) {
        d.claimsReceived[a.id] += claims_amount;
    } else {
        d.claimsReceived[a.id] = claims_amount;
    }
    insurer_lose(claims_amount);
}

void Game::insurer_recoup(float recoup) {
    insurer.gain(recoup);
    paidClaims -= recoup;
}

void Game::government_gain(float gain) {
    government.gain(gain);
}

void Game::government_lose(float loss) {
    // government.lose(loss); // TODO this is different than Python!! is this ok? check!
    if (loss > government.assets) { // TODO should the other loss functions be like this as well?
        loss = government.assets;
    }
    government.lose(loss);
}

void Game::a_distributes_loot(Attacker a) {
    caught += 1;

    for (const auto& pair : a.victims) {
        int k = pair.first;
        float v = pair.second;

        if (a.assets > 0) {
            if (a.assets > v) {
                d_recoup(a, defenders[k], v);
                a_lose(a, v);
            } else {
                d_recoup(a, defenders[k], a.assets);
                a_lose(a, a.assets);
                break;
            }
        } else {
            break;
        }
    }

    if (a.assets > 0) {
        government_gain(a.assets);
        a_lose(a, a.assets);
    }
}



// TODO TODO TODO make sure you're passing by reference, not by value! Check throughout!!
void Game::fight(Attacker a, Defender d) {
    float effective_loot = d.assets * p.PAYOFF;

    // Mercy kill the Defenders if the loot is very low
    if (d.assets < p.E) {
        effective_loot = d.assets;
    }

    float cost_of_attack = d.costToAttack;
    float expected_earnings = effective_loot * d.probAttackSuccess;

    if (expected_earnings > cost_of_attack && cost_of_attack <= a.assets) {
        attacksAttempted += 1;

        a_lose(a, cost_of_attack);
        attackerExpenditures += cost_of_attack;

        float chance_of_getting_caught = (government.assets / (government.assets + a.assets)) * p.CAUGHT;
        government_lose(a.assets * p.EFFORT);

        if (uniform(gen) < chance_of_getting_caught) {
            a_distributes_loot(a);
        } else {
            bool attacker_wins = (uniform(gen) < d.probAttackSuccess);
            if (attacker_wins) {
                attacksSucceeded += 1;
                a_steals_from_d(a, d, effective_loot);

                if (insurer.assets > 0) {
                    insurer_covers_d_for_losses_from_a(a, d, effective_loot);
                }
            }
        }
    }
}

void Game::run_iterations() {

    std::cout << "in running iterations! N = " << p.N << std::endl;

    bool defenders_have_more_than_attackers = true;

    for (iter_num = 1; iter_num < p.N + 1; iter_num++) {

        defender_iter_sum = 0;
        attacker_iter_sum = 0;

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