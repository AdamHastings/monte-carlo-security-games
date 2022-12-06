
#include <set>
#include <random>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <ctime>
#include <iostream>
#include <cassert>
#include "Game.h"

#define ASSERT(condition, input) { if(!(condition)){ std::cerr << "ASSERT FAILED: " << #condition << " with input " << std::to_string(input) << " @ " << __FILE__ << " (" << __LINE__ << ")" << std::endl; } }



static std::random_device rd;  // Will be used to obtain a seed for the random number engine
static std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
std::uniform_real_distribution<> uniform(0.0, 1.0);

Game::Game(Params &prm, std::vector<Defender> &d, std::vector<Attacker> &a, Insurer &i, Government &g) {
    p = prm;
    defenders = d;
    attackers = a;
    insurer = i;
    government = g;

    d_init = 0;
    for (auto di : defenders) {
        d_init += di.get_assets();
    }

    a_init = 0;
    for (auto ai : attackers) {
        a_init += ai.get_assets();
    }

    i_init = insurer.get_assets();
    g_init = government.get_assets();

    for (int i=0; i<attackers.size(); i++) {
        alive_attackers.insert(i);
    }
    
    for (int i=0; i<defenders.size(); i++) {
        alive_defenders.insert(i);
    }

    outside_epsilon_count_defenders = p.D;
    outside_epsilon_count_attackers = p.D;

    current_defender_sum_assets = d_init;
    current_attacker_sum_assets = a_init;

    last_delta_defenders_changes.assign(p.D, p.E);
    last_delta_attackers_changes.assign(p.D, p.E);

    if (p.verbose) {
        defenders_cumulative_assets.push_back(d_init);
        attackers_cumulative_assets.push_back(a_init);
        insurer_cumulative_assets.push_back(i_init);
        government_cumulative_assets.push_back(g_init);
    }

}

void Game::verify_state() {
    assert(round(current_defender_sum_assets) >= 0);
    assert(round(current_attacker_sum_assets) >= 0);
    // assert((round(insurer.assets)  == 0) && (std::to_string(insurer.assets) != ""));
    // ASSERT((round(insurer.assets)  >= 0), insurer.assets)
    assert(round(insurer.assets) >= 0);
    assert(round(government.assets) >= 0);

    double checksum_attacker_sum_assets = 0;
    for (auto a : attackers) {
        assert(round(a.assets) >= 0);
        checksum_attacker_sum_assets += a.assets;
    }
    // std::cout << current_attacker_sum_assets << " " << checksum_attacker_sum_assets << std::endl;
    // TODO this seems like it will always pass no matter what...? look into it...
    // if (round(current_attacker_sum_assets - checksum_attacker_sum_assets) != 0) {
    //     std::cout << "  a_init: " << a_init << std::endl;
    //     std::cout << "  current_attacker_sum_assets: " << current_attacker_sum_assets << std::endl;
    //     std::cout << "  checksum_attacker_sum_assets: " << checksum_attacker_sum_assets << std::endl;
    // }
    assert(round(current_attacker_sum_assets - checksum_attacker_sum_assets) == 0);


    double checksum_defender_sum_assets = 0;
    for (auto d : defenders) {
        assert(round(d.assets) >= 0);
        checksum_defender_sum_assets += d.assets;
    }

    // if (round(current_defender_sum_assets - checksum_defender_sum_assets) != 0) {
    //     std::cout << "  d_init: " << d_init << std::endl;
    //     std::cout << "  current_defender_sum_assets: " << current_defender_sum_assets << std::endl;
    //     std::cout << "  checksum_defender_sum_assets: " << checksum_defender_sum_assets << std::endl;
    //      std::cout << "  difference: " << round(current_defender_sum_assets - checksum_defender_sum_assets) << std::endl;

    // }
    assert(round(current_defender_sum_assets - checksum_defender_sum_assets) == 0);

    assert(round(i_init - insurer.assets) >= 0);
    assert(round(d_init - current_defender_sum_assets) >= 0);
    assert(round(i_init - paidClaims) >= 0);
    assert(round(paidClaims) >= 0);
    assert(round(attackerLoots - paidClaims) >= 0);

    if (final_outcome == "E") {
        assert(alive_attackers.size() > 0);
        assert(alive_defenders.size() > 0);
        assert(iter_num >= p.D); 
    } else if (final_outcome == "D") {
        assert(alive_defenders.size() == 0);
    } else if (final_outcome == "A") {
        assert(alive_attackers.size() == 0);
    }

    double init_ = d_init + a_init + g_init + i_init;
    double end_  = current_defender_sum_assets + current_attacker_sum_assets + insurer.assets + government.assets + attackerExpenditures + governmentExpenditures;


    if (round(init_ - end_) != 0) {
        std::cout << init_ << " " << end_ << std::endl;
    }
    assert(round(init_ - end_) == 0);
    


    return;
    
}

void Game::conclude_game(std::string outcome) {
    final_outcome = outcome;
    // std::cout << "concluding with outcome: " << outcome << std::endl;
    verify_state();
}

bool Game::is_equilibrium_reached() {
    double last_delta_defenders_pop = last_delta_defenders_changes[iter_num % p.D];
    double last_delta_attackers_pop = last_delta_attackers_changes[iter_num % p.D];
    last_delta_defenders_changes[iter_num % p.D] = defender_iter_sum;
    last_delta_attackers_changes[iter_num % p.D] = attacker_iter_sum;

    if (defender_iter_sum >= p.E) {
        if (last_delta_defenders_pop < p.E) {
            outside_epsilon_count_defenders += 1;
        }
    } else {
        if (last_delta_defenders_pop >= p.E) {
            outside_epsilon_count_defenders -= 1;
        }
    }

    if (attacker_iter_sum >= p.E) {
        if (last_delta_attackers_pop < p.E) {
            outside_epsilon_count_attackers += 1;
        }
    } else {
        if (last_delta_attackers_pop >= p.E) {
            outside_epsilon_count_attackers -= 1;
        }
    }

    // TODO add some asserts?
    // Don't understand this part..
    return ((outside_epsilon_count_attackers == 0) && (outside_epsilon_count_defenders == 0));
}

void Game::a_steals_from_d(Attacker &a, Defender &d, double loot) {
    d_lose(d, loot);
    // std::cout << " Attacker " << a.id << " is stealing " << loot << " from Defender " << d.id << std::endl;
    // std::cout << "   assets before: " << a.assets << std::endl;
    a_gain(a, loot);
    // std::cout << "   assets after:  " << a.assets << std::endl;
    attackerLoots += loot;

    // Check if this d has previously been attacked by a
    if (a.victims.find(d.id) != a.victims.end()) {
        a.victims[d.id] += loot;
    } else {
        a.victims[d.id] = loot;
    }
}

void Game::d_gain(Defender &d, double gain) {
    d.gain(gain);
    defender_iter_sum += gain;
}

void Game::d_lose(Defender &d, double loss) {
    // std::cout << " -> Defender " << d.id << " losing " << loss << std::endl;
    d.lose(loss);
    defender_iter_sum -= loss;
}

void Game::a_gain(Attacker &a, double gain) {
    // std::cout << " ** Attacker " << a.id << " gaining " << gain << std::endl;
    a.gain(gain);
    attacker_iter_sum += gain;
}

void Game::a_lose(Attacker &a, double loss) {
    // std::cout << " ** Attacker " << a.id << " losing " << loss << std::endl;
    a.lose(loss);
    attacker_iter_sum -= loss;
}

void Game::d_recoup(Attacker &a, Defender &d, double recoup_amount) {
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

void Game::insurer_lose(double loss) {
    insurer.lose(loss);
    paidClaims += loss;
}

void Game::insurer_covers_d_for_losses_from_a(Attacker &a, Defender &d, double claim) {
    double claims_amount = claim * p.CLAIMS;
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

void Game::insurer_recoup(double recoup) {
    insurer.gain(recoup);
    paidClaims -= recoup;
}

void Game::government_gain(double gain) {
    government.gain(gain);
}

void Game::government_lose(double loss) {
    government.lose(loss);
}

void Game::a_distributes_loot(Attacker &a) {
    caught += 1;

    for (const auto& pair : a.victims) {
        int k = pair.first;
        double v = pair.second;

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
void Game::fight(Attacker &a, Defender &d) {

    // std::cout << "Attacker " << a.id << " fighting Defender " << d.id << std::endl;

    double effective_loot = d.assets * p.PAYOFF;

    // Mercy kill the Defenders if the loot is very low
    if (d.assets < p.E) {
        effective_loot = d.assets;
    }

    double cost_of_attack = d.costToAttack;
    double expected_earnings = effective_loot * d.probAttackSuccess;

    // Note slight logic change
    if (expected_earnings > cost_of_attack && cost_of_attack <= a.assets) {
        attacksAttempted += 1;
        // std::cout << " -- a attacks" << std::endl;
        a_lose(a, cost_of_attack);
        attackerExpenditures += cost_of_attack;

        double chance_of_getting_caught = (government.assets / (government.assets + a.assets)) * p.CAUGHT;
        
        double gov_cost = a.assets * p.EFFORT;
        if (gov_cost > government.assets) {
            gov_cost = government.assets;
        }
        government_lose(gov_cost);
        governmentExpenditures += gov_cost;

        if (uniform(gen) < chance_of_getting_caught) {
            // std::cout << " -- a is caught!" << std::endl;
            a_distributes_loot(a);
        } else {
            bool attacker_wins = (uniform(gen) < d.probAttackSuccess);
            if (attacker_wins) {
                // std::cout << " -- a wins!" << std::endl;
                attacksSucceeded += 1;
                a_steals_from_d(a, d, effective_loot);

                if (insurer.assets > 0) {
                    insurer_covers_d_for_losses_from_a(a, d, effective_loot);
                }
            }
        }
    } // else {
    //     std::cout << " -- a does NOT attack" << std::endl;
    // }
}

void Game::run_iterations() {

    // std::cout << "in running iterations! N = " << p.N << std::endl;

    bool defenders_have_more_than_attackers = true;

    // std::cout << p.to_string() << std::endl;

    // std::cout << " attackers: ";
    // for (auto i : alive_attackers) {
    //     std::cout << attackers[i].id << ", ";
    // }
    // std::cout << std::endl;

    // std::cout << " defenders: ";
    // for (auto i : alive_defenders) {
    //     std::cout << defenders[i].id << ", ";
    // }
    // std::cout << std::endl;

    // for (auto d : defenders) {
    //     if (round(d.assets) < 0) {
    //         std::cout << " ------ Defender " << d.id << " has " << d.assets << std::endl;
    //     }
    // }

    for (iter_num = 1; iter_num < p.N + 1; iter_num++) {

        defender_iter_sum = 0;
        attacker_iter_sum = 0;

        // std::cout << " -------- Round " << iter_num << " -----------" << std::endl;

        std::vector<int> alive_attackers_list(alive_attackers.begin(), alive_attackers.end()); // TODO maybe optimize this later
        std::vector<int> alive_defenders_list(alive_defenders.begin(), alive_defenders.end()); // TODO maybe optimize this later

        std::random_device rd;
        std::mt19937 g(rd());

        int shorter_length;
        if (alive_defenders_list.size() < alive_attackers_list.size()) {
            std::shuffle(alive_attackers_list.begin(), alive_attackers_list.end(), g);
            shorter_length = alive_defenders_list.size();
        } else {
            std::shuffle(alive_defenders_list.begin(), alive_defenders_list.end(), g);
            shorter_length = alive_attackers_list.size();
        }

        for (int i=0; i<shorter_length; i++) {
            Attacker *a = &attackers[alive_attackers_list[i]];
            Defender *d = &defenders[alive_defenders_list[i]];
            fight(*a, *d);

            if (std::round(attackers[i].assets) <= 0) {
                alive_attackers.erase(alive_attackers_list[i]);
            }
            if (std::round(defenders[i].assets) <= 0) {
                alive_defenders.erase(alive_defenders_list[i]);
            }
        }

        current_defender_sum_assets += defender_iter_sum;
        current_attacker_sum_assets += attacker_iter_sum;

        double init_ = d_init + a_init + g_init + i_init;
        double end_  = current_defender_sum_assets + current_attacker_sum_assets + insurer.assets + government.assets + attackerExpenditures + governmentExpenditures;


        if (round(init_ - end_) != 0) {
            std::cout << "checksum failed! " << init_ << " " << end_ << std::endl;
            std::cout << d_init << " " << current_defender_sum_assets << std::endl;
            std::cout << a_init << " " << current_attacker_sum_assets << " " << attackerExpenditures << std::endl;
            std::cout << i_init << " " << insurer.assets << std::endl;
            std::cout << g_init << " " << government.assets << std::endl;
        }
        assert(round(init_ - end_) == 0);
    

        // std::cout << " -> current_attacker_sum_assets: " << current_attacker_sum_assets << std::endl;

        /////////////////////
        // double checksum_defender_sum_assets = 0;
        // for (auto d : defenders) {
        //     if (round(d.assets) < 0) {
        //         std::cout << "Defender " << d.id << " has " << d.assets << std::endl;
        //     }

        //     assert(round(d.assets) >= 0);
        //     checksum_defender_sum_assets += d.assets;
        // }
        
        // if (round(current_defender_sum_assets - checksum_defender_sum_assets) != 0) {
        //     std::cout << "  d_init: " << d_init << std::endl;
        //     std::cout << "  current_defender_sum_assets: " << current_defender_sum_assets << std::endl;
        //     std::cout << "  checksum_defender_sum_assets: " << checksum_defender_sum_assets << std::endl;
        //     std::cout << "  difference: " << round(current_defender_sum_assets - checksum_defender_sum_assets) << std::endl;

        // }
        // assert(round(current_defender_sum_assets - checksum_defender_sum_assets) == 0);
        //////////////


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