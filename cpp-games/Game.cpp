
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
// static std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
static std::mt19937 gen(0);
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
        alive_attackers_indices.push_back(i);
    }

    for (uint i=0; i<defenders.size(); i++) {
        alive_defenders_indices.push_back(i);
    }
    

    outside_epsilon_count_defenders = p.D;
    outside_epsilon_count_attackers = p.D;

    current_defender_sum_assets = d_init;
    current_attacker_sum_assets = a_init;

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

    ret += std::to_string(p.MANDATE).substr(0,4) + ",";
    ret += std::to_string(p.ATTACKERS).substr(0,4) + ",";
    ret += std::to_string(p.INEQUALITY).substr(0,4) + ",";
    ret += std::to_string(p.PREMIUM).substr(0,4) + ",";
    ret += std::to_string(p.EFFICIENCY).substr(0,4) + ",";
    ret += std::to_string(p.EFFORT).substr(0,4) + ",";
    ret += std::to_string(p.PAYOFF).substr(0,4) + ",";
    ret += std::to_string(p.CAUGHT).substr(0,4) + ",";
    ret += std::to_string(p.CLAIMS).substr(0,4) + ",";
    ret += std::to_string(p.TAX).substr(0,4) + ",";
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
    uint i = 0;
    for (auto d : defenders) {
        assert(d.id >= 0);
        assert(d.id < defenders.size());
        assert(d.assets >= 0);
        assert(d.costToAttack >= 0);
        assert(d.posture >= 0);
        assert(d.posture <= 1);
        assert(d.id == i);
        i++;
    }

    i = 0;
    for (auto a : attackers) {
        assert(a.id >= 0);
        assert(a.id < attackers.size());
        assert(a.assets >= 0);
        assert(a.id == i);
        i++;
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
        assert(alive_attackers_indices.size() > 0);
        assert(alive_defenders_indices.size() > 0);
        assert(iter_num >= p.D); 
    } else if (final_outcome == "D") {
        assert(alive_defenders_indices.size() == 0);
    } else if (final_outcome == "A") {
        assert(alive_attackers_indices.size() == 0);
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
    verify_outcome();
}

bool Game::is_equilibrium_reached() {


    if (roundAttacks == 0) {
        consecutiveNoAttacks++;
    } else {
        consecutiveNoAttacks = 0;
    }

    return (consecutiveNoAttacks >= p.D);
}

void Game::a_steals_from_d(Attacker &a, Defender &d, double loot) {
    d_lose(d, loot);
    a_gain(a, loot);
    attackerLoots += loot;

    // Check if this d has previously been attacked by a
    if (a.victims.find(d.id) != a.victims.end()) {
        a.victims[d.id] += loot;
    } else {    
        a.victims.insert({d.id, loot});
    }
}

void Game::d_gain(Defender &d, double gain) {
    d.gain(gain);
    defender_iter_sum += gain;
}

void Game::d_lose(Defender &d, double loss) {
    d.lose(loss);
    defender_iter_sum -= loss;

}

void Game::a_gain(Attacker &a, double gain) {
    a.gain(gain);
    attacker_iter_sum += gain;
}

void Game::a_lose(Attacker &a, double loss) {
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
            if (std::find(alive_defenders_indices.begin(), alive_defenders_indices.end(), d.id) == alive_defenders_indices.end()) {
                alive_defenders_indices.push_back(d.id);
            }
        } else {
            insurer_recoup(recoup_amount);
            d.claimsReceived[a.id] -= recoup_amount;
        }
    } else {
        d_gain(d, recoup_amount);

        if (std::find(alive_defenders_indices.begin(), alive_defenders_indices.end(), d.id) == alive_defenders_indices.end()) {
            alive_defenders_indices.push_back(d.id);
        }
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



void Game::fight(Attacker &a, Defender &d) {
    double effective_loot = d.assets * p.PAYOFF;

    // Mercy kill the Defenders if the loot is very low
    if (d.assets < p.E) {
        effective_loot = d.assets;
    }

    double cost_of_attack = d.costToAttack;

    double expected_earnings = effective_loot * d.posture; // TODO wrong

    if (expected_earnings > cost_of_attack && cost_of_attack <= a.assets) {
        attacksAttempted += 1;
        roundAttacks += 1;
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
            a_distributes_loot(a);
        } else {
            bool attacker_wins = (uniform(gen) < d.posture); // TODO wrong
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

// double Game::find_p_attacker_has_enough_to_attack() {
//     // probably need to do a binary search of the sorted attacker assets distribution
//     return 0; // TODO
// }



// void Game::requeset_a_quote(Defender &d) {
//     double a2d_ratio = attackers.size() / defenders.size();
//     double p_attacker_has_enough_to_attack = find_p_attacker_has_enough_to_attack();
//     double p_getting_attacked = std::min(1.0, a2d_ratio);
//     double p_a = p_getting_attacked * p_attacker_has_enough_to_attack;

//     double p_a_hat = prevRoundAttacks / 

// }

void Game::run_iterations() {

    bool defenders_have_more_than_attackers = true;

    for (iter_num = 1; iter_num < p.N + 1; iter_num++) {


        defender_iter_sum = 0;
        attacker_iter_sum = 0;
        roundAttacks = 0;

        std::vector<int> new_alive_defenders_indices;
        std::vector<int> new_alive_attackers_indices;

        uint shorter_length, offset;
        bool more_defenders_than_attackers = (alive_defenders_indices.size() > alive_attackers_indices.size());
        if (more_defenders_than_attackers) {

            std::shuffle(alive_attackers_indices.begin(), alive_attackers_indices.end(), gen);
            std::uniform_int_distribution<> distr(0, alive_defenders_indices.size() - alive_attackers_indices.size());
            offset = distr(gen);
            shorter_length = alive_attackers_indices.size();

            new_alive_defenders_indices.insert( new_alive_defenders_indices.end(), alive_defenders_indices.begin(), alive_defenders_indices.begin() + offset );
            new_alive_defenders_indices.insert( new_alive_defenders_indices.end(), alive_defenders_indices.begin() + offset + alive_attackers_indices.size(), alive_defenders_indices.end() );

        } else {
            // Shuffle smaller list
            // Then pair it up at a random point in the opposing list
            std::shuffle(alive_defenders_indices.begin(), alive_defenders_indices.end(), gen);
            std::uniform_int_distribution<> distr(0, alive_attackers_indices.size() - alive_defenders_indices.size());
            offset = distr(gen);
            shorter_length = alive_defenders_indices.size();

            new_alive_attackers_indices.insert( new_alive_attackers_indices.end(), alive_attackers_indices.begin(), alive_attackers_indices.begin() + offset );
            new_alive_attackers_indices.insert( new_alive_attackers_indices.end(), alive_attackers_indices.begin() + offset + alive_defenders_indices.size(), alive_attackers_indices.end() );

        }

        for (auto d : defenders) {
            d.choose_security_strategy();
        }

        
        for (uint i=0; i<shorter_length; i++) {
            uint a_idx, d_idx;
            if (more_defenders_than_attackers) {
                a_idx = alive_attackers_indices[i];
                d_idx = alive_defenders_indices[i + offset];
            } else {
                a_idx = alive_attackers_indices[i + offset];
                d_idx = alive_defenders_indices[i];
            }

            Attacker *a = &attackers[a_idx];
            Defender *d = &defenders[d_idx];

            // buy_insurance(*d); 
            // d needs to pick its optimal strategy here
            // or maybe beforehand? Before the round so the insurer can collect

            fight(*a, *d);

            if (std::round(a->assets) > 0) {
                new_alive_attackers_indices.push_back(a_idx);
            }
            if (std::round(d->assets) > 0) {
                new_alive_defenders_indices.push_back(d_idx);
            }
        }

        prevRoundAttacks = roundAttacks;

        alive_attackers_indices = new_alive_attackers_indices;
        alive_defenders_indices = new_alive_defenders_indices;

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

        if (alive_attackers_indices.size() == 0) {
            conclude_game("A");
            return;
        } else if (alive_defenders_indices.size() == 0) {
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