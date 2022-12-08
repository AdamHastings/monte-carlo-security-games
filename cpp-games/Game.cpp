
#include <set>
#include <random>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <ctime>
#include <iostream>
#include <cassert>
#include <string.h>
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

    for (uint i=0; i<attackers.size(); i++) {
        alive_attackers.insert(i);
    }
    
    for (uint i=0; i<defenders.size(); i++) {
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

    // Let's make sure everything got set up correctly
    if (p.assertions_on) {
        verify_init();  
    }
}

std::string Game::to_string() {
    std::string ret = "";

    ret += std::to_string(p.MANDATE).substr(0,3) + ",";
    ret += std::to_string(p.ATTACKERS).substr(0,3) + ",";
    ret += std::to_string(p.INEQUALITY).substr(0,3) + ",";
    ret += std::to_string(p.PREMIUM).substr(0,3) + ",";
    ret += std::to_string(p.EFFICIENCY).substr(0,3) + ",";
    ret += std::to_string(p.EFFORT).substr(0,3) + ",";
    ret += std::to_string(p.PAYOFF).substr(0,3) + ",";
    ret += std::to_string(p.CAUGHT).substr(0,3) + ",";
    ret += std::to_string(p.CLAIMS).substr(0,3) + ",";
    ret += std::to_string(p.TAX).substr(0,3) + ",";
    ret += std::to_string(int(round(d_init))) + ",";
    ret += std::to_string(int(round(current_defender_sum_assets))) + ",";
    ret += std::to_string(int(round(a_init))) + ",";
    ret += std::to_string(int(round(current_attacker_sum_assets))) + ",";
    ret += std::to_string(int(round(i_init))) + ",";
    ret += std::to_string(int(round(insurer.assets))) + ",";
    ret += std::to_string(int(round(g_init))) + ",";
    ret += std::to_string(int(round(government.assets))) + ",";
    ret += std::to_string(int(round(attacksAttempted))) + ",";
    ret += std::to_string(int(round(attacksSucceeded))) + ",";
    ret += std::to_string(int(round(attackerLoots))) + ",";
    ret += std::to_string(int(round(attackerExpenditures))) + ",";
    ret += std::to_string(int(round(governmentExpenditures))) + ",";
    ret += "\"[";
    for (auto i : crossovers) {
        ret += std::to_string(i) + ",";
    }
    ret += "]\",";
    ret += "\"[";
    for (auto i : insurerTimesOfDeath) {
        ret += std::to_string(i) + ",";
    }
    ret += "]\",";
    ret += std::to_string(int(round(paidClaims))) + ",";
    ret += std::to_string(iter_num) + ",";
    ret += final_outcome;

    // TODO
    // if (verbose):
    //     ret += "\"" + str(self.defenders_cumulative_assets)  + "\","
    //     ret += "\"" + str(self.attackers_cumulative_assets)  + "\","
    //     ret += "\"" + str(self.insurer_cumulative_assets)    + "\","
    //     ret += "\"" + str(self.government_cumulative_assets) + "\","
    
    ret += "\n";
    return ret;

}

void Game::verify_init() {

    // TODO store each D's init conditions? To compare against later...?
    for (auto d : defenders) {
        assert(d.id >= 0);
        assert(d.id < defenders.size());
        assert(d.assets >= 0);
        assert(d.costToAttack >= 0);
        assert(d.probAttackSuccess >= 0);
        assert(d.probAttackSuccess <= 1);
    }

    for (auto a : attackers) {
        assert(a.id >= 0);
        assert(a.id < attackers.size());
        assert(a.assets >= 0);
    }

    assert(insurer.assets >= 0);
    assert(government.assets >= 0);
}


void Game::verify_outcome() {
    assert(round(current_defender_sum_assets) >= 0);
    assert(round(current_attacker_sum_assets) >= 0);
    assert(round(insurer.assets) >= 0);
    assert(round(government.assets) >= 0);

    double checksum_attacker_sum_assets = 0;
    for (auto a : attackers) {
        assert(round(a.assets) >= 0);
        checksum_attacker_sum_assets += a.assets;
    }

    assert(round(current_attacker_sum_assets - checksum_attacker_sum_assets) == 0);

    double checksum_defender_sum_assets = 0;
    for (auto d : defenders) {
        assert(round(d.assets) >= 0);
        checksum_defender_sum_assets += d.assets;
    }

    assert(round(current_defender_sum_assets - checksum_defender_sum_assets) == 0);

    assert(round(i_init - insurer.assets) >= 0);
    // assert(round(d_init - current_defender_sum_assets) >= 0); // This might actually not be the case! E.g. all defender losses have been covered, and an attacker who received no claims then gets recouped.
    assert(round(i_init - paidClaims) >= 0);
    assert(round(paidClaims) >= 0);
    assert(round(attackerLoots - paidClaims) >= 0);
    assert(attackerExpenditures >= 0);
    assert(governmentExpenditures >= 0);

    if (final_outcome == "E") {
        assert(alive_attackers.size() > 0);
        assert(alive_defenders.size() > 0);
        assert(iter_num >= p.D); 
    } else if (final_outcome == "D") {
        assert(alive_defenders.size() == 0);
    } else if (final_outcome == "A") {
        assert(alive_attackers.size() == 0);
    }

    // Master checksum
    double init_ = d_init + a_init + g_init + i_init;
    double end_  = current_defender_sum_assets + current_attacker_sum_assets + insurer.assets + government.assets + attackerExpenditures + governmentExpenditures;

    if (round(init_ - end_) != 0) {
        std::cout << init_ << " " << end_ << std::endl;
    }
    assert(round(init_ - end_) == 0);
  
}

void Game::conclude_game(std::string outcome) {
    final_outcome = outcome;
    // std::cout << "concluding with outcome: " << outcome << std::endl;
    verify_outcome();
}

bool Game::is_equilibrium_reached() {
    // std::cout << "checking equilibrium...";
    double last_delta_defenders_pop = last_delta_defenders_changes[iter_num % p.D];
    double last_delta_attackers_pop = last_delta_attackers_changes[iter_num % p.D];
    last_delta_defenders_changes[iter_num % p.D] = defender_iter_sum;
    last_delta_attackers_changes[iter_num % p.D] = attacker_iter_sum;

    // std::cout << "sums: " << defender_iter_sum << ", " << attacker_iter_sum << std::endl;
    // std::cout << "pops: " << last_delta_attackers_pop << ", " << last_delta_attackers_pop << std::endl;
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
    // std::cout << "done!" << std::endl;
    // std::cout << outside_epsilon_count_attackers << ", " << outside_epsilon_count_defenders << std::endl;

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

    // return;
    // Check if this d has previously been attacked by a
    if (a.victims.find(d.id) != a.victims.end()) {
        // std::cout << "found!\n";
        a.victims[d.id] += loot;
    } else {
        // std::cout << "not found!\n";
        // return;
        // std::cout << a.victims.size() << std::endl;

        // if (d.id > defenders.size()) {
        //     std::cout << "~~~~ gonna cause a big problem! d.id = " << d.id << std::endl;
        //     return;
        // }
        // return;
        
        a.victims.insert({d.id, loot});
        // } catch(std::bad_alloc& ex) {
        // std::cerr << " Out of memory!";
        // cin.get();
        // exit(1);

        // std::cout << "done!\n";
    }
}

void Game::d_gain(Defender &d, double gain) {
    d.gain(gain);
    defender_iter_sum += gain;
    // std::cout << "defender_iter_sum = " << defender_iter_sum << std::endl;
}

void Game::d_lose(Defender &d, double loss) {
    // std::cout << " -> Defender " << d.id << " losing " << loss << std::endl;
    d.lose(loss);
    defender_iter_sum -= loss;
    // std::cout << "defender_iter_sum = " << defender_iter_sum << " (loss)" << std::endl;

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

        // std::cout << "    defender[" << d.id << "] has claimed " << d.claimsReceived[a.id] << " and has lost " << a.victims[d.id] << std::endl;

        if (recoup_amount >= d.claimsReceived[a.id]) {
            // std::cout << "      gets to keep " << recoup_amount - d.claimsReceived[a.id] << std::endl;
            // std::cout << "      insurer keeps " << d.claimsReceived[a.id] << std::endl;
            d_gain(d, recoup_amount - d.claimsReceived[a.id]);
            insurer_recoup(d.claimsReceived[a.id]);
            d.claimsReceived[a.id] = 0;
            alive_defenders.insert(d.id);
        } else {
            // std::cout << "      insurer gets all  " << recoup_amount << std::endl;
            insurer_recoup(recoup_amount);
            d.claimsReceived[a.id] -= recoup_amount;
        }
    } else {

        // std::cout << "    defender has not claimed anything and gets everything" << std::endl;
        alive_defenders.insert(d.id);
        d_gain(d, recoup_amount);
    }
    // assert(round(d_init - current_defender_sum_assets) >= 0);
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
    // return;

    for (const auto& pair : a.victims) {
        int k = pair.first;
        double v = pair.second;

        // std::cout << "  attacker[" << a.id << "] distributing " << v << " to defender[" << k << "]" << std::endl;
        // if (k > defenders.size()) {
        //     std::cout << "whoaa big problem! k = " << k << std::endl;
        //     return;
        // }

        if (a.assets > 0) {
            if (a.assets > v) {
                // std::cout << "    a.assets > v" << std::endl;
                d_recoup(a, defenders[k], v);
                a_lose(a, v);
            } else {
                // std::cout << "    a.assets <= v" << std::endl;
                d_recoup(a, defenders[k], a.assets);
                a_lose(a, a.assets);
                break;
            }
        } else {
            // std::cout << "    all out of recoup juice" << std::endl;
            break;
        }
        // assert(round(d_init - current_defender_sum_assets) >= 0);

    }

    if (a.assets > 0) {
        government_gain(a.assets);
        a_lose(a, a.assets);
    }

    // assert(round(d_init - current_defender_sum_assets) >= 0);
    // std::cout << "done distributing" << std::endl;
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
                // if (d.id > defenders.size()) {
                //     std::cout << " --- trouble brewing...\n";
                // } else {
                //     std::cout << "  A OK\n";
                // }
                a_steals_from_d(a, d, effective_loot);
                if (insurer.assets > 0) {
                    // std::cout << " -- insurer covers losses" << std::endl;
                    insurer_covers_d_for_losses_from_a(a, d, effective_loot);
                }
            }
        }
    } // else {
    //     std::cout << " -- a does NOT attack" << std::endl;
    // }
}

void Game::run_iterations() {

    // std::cout << p.to_string() << std::endl;

    bool defenders_have_more_than_attackers = true;


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
            // TODO shuffle sampels instead of full array
            std::shuffle(alive_attackers_list.begin(), alive_attackers_list.end(), g);
            shorter_length = alive_defenders_list.size();
        } else {
            // TODO shuffle samples instead of full array
            std::shuffle(alive_defenders_list.begin(), alive_defenders_list.end(), g);
            shorter_length = alive_attackers_list.size();
        }

        for (int i=0; i<shorter_length; i++) {
            Attacker *a = &attackers[alive_attackers_list[i]];
            Defender *d = &defenders[alive_defenders_list[i]];
            fight(*a, *d);

            if (std::round(a->assets) <= 0) {
                alive_attackers.erase(alive_attackers_list[i]);
            }
            if (std::round(d->assets) <= 0) {
                alive_defenders.erase(alive_defenders_list[i]);
            }
        }
        // std::cout << "fights done\n";

        current_defender_sum_assets += defender_iter_sum;
        current_attacker_sum_assets += attacker_iter_sum;



        // double init_ = d_init + a_init + g_init + i_init;
        // double end_  = current_defender_sum_assets + current_attacker_sum_assets + insurer.assets + government.assets + attackerExpenditures + governmentExpenditures;


        // if (round(init_ - end_) != 0) {
        //     std::cout << "checksum failed! " << init_ << " " << end_ << std::endl;
        //     std::cout << d_init << " " << current_defender_sum_assets << std::endl;
        //     std::cout << a_init << " " << current_attacker_sum_assets << " " << attackerExpenditures << std::endl;
        //     std::cout << i_init << " " << insurer.assets << std::endl;
        //     std::cout << g_init << " " << government.assets << std::endl;
        // }
        // assert(round(init_ - end_) == 0);

        // std::cout << d_init << " " << current_defender_sum_assets << " " << defender_iter_sum << std::endl;
        
        // // new assert
        // assert(round(d_init - current_defender_sum_assets) >= 0);
    

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

        // std::cout << "done with adding crossovers" << std::endl;

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
            // std::cout << "equilibrium!!!!" << std::endl;
            conclude_game("E");
            return;
        }

    }

    conclude_game("N");
    return;
}