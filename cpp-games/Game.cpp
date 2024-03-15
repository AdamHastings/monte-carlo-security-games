
#include <set>
#include <random>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <ctime>
#include <iostream>
#include <cassert>
#include <string.h>
#include <vector>
#include "Game.h"

static std::random_device rd;  // Will be used to obtain a seed for the random number engine
// static std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
static std::mt19937 gen(0); // TODO undo static seed?
std::uniform_real_distribution<> uniform(0.0, 1.0);

Game::Game(Params prm) {
    p = prm;

    for (int j=0; j < p.NUM_INSURERS; j++) {
        Insurer i = Insurer(j, p);
        insurers.push_back(i);
    }

    for (int i=0; i < p.NUM_BLUE_PLAYERS; i++) {
        Defender d = Defender(i, p, insurers);
        defenders.push_back(d);
        alive_defenders_indices.push_back(i);
    }

    std::cout << p.ATTACKERS << std::endl;
    for (int i=0; i < (p.NUM_BLUE_PLAYERS * p.ATTACKERS); i++) { // TODO isn't this wrong now because p.ATTACKERS is a distribution?
        Attacker a = Attacker(i, p);
        attackers.push_back(a);
        alive_attackers_indices.push_back(i);
    }
    
    outside_epsilon_count_defenders = p.DELTA;
    outside_epsilon_count_attackers = p.DELTA;

    Defender::current_sum_assets = Defender::d_init;
    Attacker::current_sum_assets = Attacker::a_init;
    Insurer::current_sum_assets = Insurer::i_init;
    
    if (p.verbose) {
        Defender::cumulative_assets.push_back(Defender::d_init);
        Attacker::cumulative_assets.push_back(Attacker::Attacker::a_init);
        Insurer::cumulative_assets.push_back(Insurer::i_init);
    }


    // Let's make sure everything got set up correctly
    if (p.assertions_on) {
        verify_init();  
    }   
}

std::string Game::to_string() {
    std::string ret = "";

    ret += std::to_string(p.ATTACKERS).substr(0,4) + ",";
    ret += std::to_string(p.INEQUALITY).substr(0,4) + ",";
    ret += std::to_string(int(round(Defender::d_init))) + ",";
    ret += std::to_string(int(round(Defender::current_sum_assets))) + ",";
    ret += std::to_string(int(round(Attacker::Attacker::a_init))) + ",";
    ret += std::to_string(int(round(Attacker::current_sum_assets))) + ",";
    ret += std::to_string(int(round(Insurer::i_init))) + ",";
    ret += std::to_string(int(round(Insurer::current_sum_assets))) + ",";
    ret += std::to_string(int(round(attacksAttempted))) + ",";
    ret += std::to_string(int(round(attacksSucceeded))) + ",";
    ret += std::to_string(int(round(attackerLoots))) + ",";
    ret += std::to_string(int(round(attackerExpenditures))) + ",";
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

    i = 0;
    for (auto ins : insurers) {
        assert(ins.id >= 0);
        assert(ins.id < insurers.size());
        assert(ins.assets >= 0);
        assert(ins.id == i);
        i++;
    }
}

void Game::verify_outcome() {
    assert(round(Defender::current_sum_assets) >= 0);
    assert(round(Attacker::current_sum_assets) >= 0);
    assert(round(Insurer::current_sum_assets)  >= 0); 

    double checksum_attacker_sum_assets = 0;
    for (auto a : attackers) {
        assert(round(a.assets) >= 0);
        checksum_attacker_sum_assets += a.assets;
    }

    assert(round(Attacker::current_sum_assets - checksum_attacker_sum_assets) == 0);

    double checksum_defender_sum_assets = 0;
    for (auto d : defenders) {
        assert(round(d.assets) >= 0);
        checksum_defender_sum_assets += d.assets;
    }

    assert(round(Defender::current_sum_assets - checksum_defender_sum_assets) == 0);

    double checksum_insurer_sum_assets = 0;
    for (auto i : insurers) {
        assert(round(i.assets) >= 0);
        checksum_insurer_sum_assets += i.assets;
    }


    assert(round(Insurer::current_sum_assets - checksum_insurer_sum_assets) >= 0);


    // assert(round(Defender::d_init - current_defender_sum_assets) >= 0); // This might actually not be the case! E.g. all defender losses have been covered, and an attacker who received no claims then gets recouped.
    assert(round(Insurer::i_init - paidClaims) >= 0);
    assert(round(paidClaims) >= 0);
    assert(round(attackerLoots - paidClaims) >= 0);
    assert(attackerExpenditures >= 0);

    if (final_outcome == "E") {
        assert(alive_attackers_indices.size() > 0);
        assert(alive_defenders_indices.size() > 0);
        assert(iter_num >= p.DELTA); 
    } else if (final_outcome == "D") {
        assert(alive_defenders_indices.size() == 0);
    } else if (final_outcome == "A") {
        assert(alive_attackers_indices.size() == 0);
    }

    // Master checksum
    double init_ = Defender::d_init + Attacker::Attacker::a_init + Insurer::i_init; 
    double end_  = Defender::current_sum_assets + Attacker::current_sum_assets + Insurer::current_sum_assets + attackerExpenditures; 

    // TODO add back in after fixing insurer
    if (round(init_ - end_) != 0) {
        std::cout << init_ << " " << end_ << std::endl;
    }
    assert(round(init_ - end_) == 0); // TODO add back in after fixing insurer
  
}

void Game::conclude_game(std::string outcome) {
    final_outcome = outcome;

    // TODO cleanup allocated pointers?

    verify_outcome();
}

bool Game::is_equilibrium_reached() {
    if (roundAttacks == 0) {
        consecutiveNoAttacks++;
    } else {
        consecutiveNoAttacks = 0;
    }

    return (consecutiveNoAttacks >= p.DELTA);
}

// void Game::a_steals_from_d(Attacker &a, Defender &d, double loot) {
//     d_lose(d, loot);
//     a_gain(a, loot);
//     attackerLoots += loot;

//     // Check if this d has previously been attacked by a
//     // TODO are we going to remove this anyway?
//     //if (a.victims.find(d.id) != a.victims.end()) {
//     //    a.victims[d.id] += loot;
//     //} else {    
//     //    a.victims.insert({d.id, loot});
//     //}
// }



// void Game::d_recoup(Attacker &a, Defender &d, double recoup_amount) {

//     if (d.claimsReceived.find(a.id) != d.claimsReceived.end()) {
//         // Split the recoup amount between the defender and the Insurer

//         if (recoup_amount >= d.claimsReceived[a.id]) {

//             d_gain(d, recoup_amount - d.claimsReceived[a.id]);
//             insurer_recoup(d.claimsReceived[a.id]);
//             d.claimsReceived[a.id] = 0;
//             if (std::find(alive_defenders_indices.begin(), alive_defenders_indices.end(), d.id) == alive_defenders_indices.end()) {
//                 alive_defenders_indices.push_back(d.id);
//             }
//         } else {
//             insurer_recoup(recoup_amount);
//             d.claimsReceived[a.id] -= recoup_amount;
//         }
//     } else {
//         d_gain(d, recoup_amount);

//         if (std::find(alive_defenders_indices.begin(), alive_defenders_indices.end(), d.id) == alive_defenders_indices.end()) {
//             alive_defenders_indices.push_back(d.id);
//         }
//     }
// }

// // TODO what about multiple insurers?
// void Game::i_lose(Insurer &i, double loss) {
//     i.lose(loss);
//     paidClaims += loss;
// }

// TODO what about multiple insurers?
// void Game::insurer_covers_d_for_losses_from_a(Attacker &a, Defender &d, double claim) {
    // double claims_amount = claim * p.CLAIMS;
    // if (claims_amount > insurer.assets) {
    //     claims_amount = insurer.assets;
    //     insurerTimesOfDeath.push_back(iter_num);
    // }

    // d_gain(d, claims_amount);
    // if (d.claimsReceived.find(a.id) != d.claimsReceived.end()) {
    //     d.claimsReceived[a.id] += claims_amount;
    // } else {
    //     d.claimsReceived[a.id] = claims_amount;
    // }
    // insurer_lose(claims_amount);
// }

// void Game::insurer_recoup(double recoup) {
//     insurer.gain(recoup);
//     paidClaims -= recoup;
// }

// void Game::government_gain(double gain) {
//     government.gain(gain);
// }

// void Game::government_lose(double loss) {
//     government.lose(loss);
// }

// void Game::a_distributes_loot(Attacker &a) {
//     caught += 1;

//     for (const auto& pair : a.victims) {
//         int k = pair.first;
//         double v = pair.second;

//         if (a.assets > 0) {
//             if (a.assets > v) {
//                 d_recoup(a, defenders[k], v);
//                 a_lose(a, v);
//             } else {
//                 d_recoup(a, defenders[k], a.assets);
//                 a_lose(a, a.assets);
//                 break;
//             }
//         } else {
//             break;
//         }
//     }

//     if (a.assets > 0) {
//         government_gain(a.assets);
//         a_lose(a, a.assets);
//     }
// }



void Game::fight(Attacker &a, Defender &d) {

    double expected_loot = d.assets * p.PAYOFF_distribution->mean();

    // Mercy kill the Defenders if the loot is very low
    if (d.assets < p.EPSILON) {
        expected_loot = d.assets;
    }


    // TODO is (expected_loot > d.costToAttack) part of the underwriting process at the moment? I don't think it is
    if (expected_loot > d.costToAttack && d.costToAttack <= a.assets) {
        // Attacking is financially worth it

        
        // bookkeeping
        attacksAttempted += 1;
        roundAttacks += 1;
        a.lose(d.costToAttack);
        attackerExpenditures += d.costToAttack;


        if (RandUniformDist.draw() > d.posture) {
            attacksSucceeded += 1;

            double loot = d.assets * p.PAYOFF_distribution->draw();
            a.gain(loot);
            attackerLoots += loot;

            // a_steals_from_d(a, d, effective_loot);
            // if (insurer.assets > 0) {
            //     insurer_covers_d_for_losses_from_a(a, d, effective_loot);
            // }

            if (d.insured) {
                // insurers[0].cover_loss(d, loot); // TODO change to allow all insurers to be included 
                // d.insurer->cover_loss(d, loot);
                d.submit_claim(loot);
            } else {
                d.lose(loot);
            }
        }
    } 
}

double findPercentile(const std::vector<double>& sortedVector, double newValue) {
    // Find the rank of the new value within the sorted vector
    auto rankIterator = std::lower_bound(sortedVector.begin(), sortedVector.end(), newValue);
    int rank = std::distance(sortedVector.begin(), rankIterator);
    
    // Calculate the percentile
    double percentile = (static_cast<double>(rank) / (sortedVector.size() - 1)) * 100.0;
    
    return percentile;
}


// Insurers use their overhead to conduct operations and perform risk analysis
// As part of this, the insurers find the median assets of the attackers (TODO maybe estimate it even?)
// which informs current defender risks before writing policies.
// TODO shouldn't this be a class function of Insurer? 
void Game::perform_market_analysis(){
    // Reset attacker_assets each round 
    Insurer::attacker_assets.clear();

    // for (auto a : attackers) { // TODO shouldn't this be *alive* attacker????
    //     Insurer::attacker_assets.push_back(a.assets);
    // }
    for (auto i : alive_attackers_indices) {
        Insurer::attacker_assets.push_back(attackers[i].assets);
    }

    std::sort(Insurer::attacker_assets.begin(), Insurer::attacker_assets.end());
    Insurer::num_attackers = attackers.size();
    Insurer::num_defenders = defenders.size();

    for (auto d : defenders) {
        d.costToAttackPercentile = findPercentile(Insurer::attacker_assets, d.costToAttack);
    }

    // Defenders don't have the same visibility as the insurers but still can make some predictions about risk.
    Defender::estimated_probability_of_attack = std::min(1.0, (roundAttacks * 1.0)/(defenders.size() * 1.0));
}

void Game::run_iterations() {

    bool defenders_have_more_than_attackers = true;

    for (iter_num = 1; iter_num < p.NUM_GAMES + 1; iter_num++) {

        // TODO maybe put all of this into a "reset_round()" function
        Defender::defender_iter_sum = 0;
        Attacker::attacker_iter_sum = 0;
        Insurer::insurer_iter_sum = 0;
        roundAttacks = 0;

        std::vector<int> new_alive_defenders_indices;
        std::vector<int> new_alive_attackers_indices;


        // TODO just do simple shuffle?
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

        perform_market_analysis();

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

            fight(*a, *d);

            if (std::round(a->assets) > 0) {
                new_alive_attackers_indices.push_back(a_idx);
            }
            if (std::round(d->assets) > 0) {
                new_alive_defenders_indices.push_back(d_idx);
            }
        }

        // Insurance policy expires
        for (auto d : defenders) {
            d.ins_idx = -1;
            d.insured = false; 
        }

        for (auto di : defenders) {
            std::cout << "    post iter : insurers.size() = " << di.insurers->size() << std::endl;
        }

        prevRoundAttacks = roundAttacks;

        alive_attackers_indices = new_alive_attackers_indices;
        alive_defenders_indices = new_alive_defenders_indices;

        // TODO maybe put this into an "end_of_round_bookkeeping()" function
        // TODO maybe the current_*_sum_assets should be a static class variable as well?
        // This is obviated by handling current asset bookkeeping in the class gain/lose functions themselves
        // Defender::current_sum_assets += Defender::defender_iter_sum;
        // Attacker::current_sum_assets += Attacker::attacker_iter_sum;
        // Insurer::current_sum_assets  += Insurer::insurer_iter_sum;

        if (defenders_have_more_than_attackers) {
            if (Attacker::current_sum_assets > Defender::current_sum_assets) {
                crossovers.push_back(iter_num);
                defenders_have_more_than_attackers = false;
            }
        } else {
            if (Attacker::current_sum_assets < Defender::current_sum_assets) {
                crossovers.push_back(iter_num);
                defenders_have_more_than_attackers = true;
            }
        }

        if (p.verbose) {
            Defender::cumulative_assets.push_back(Defender::current_sum_assets);
            Attacker::cumulative_assets.push_back(Attacker::current_sum_assets);
            Insurer::cumulative_assets.push_back(Insurer::current_sum_assets); // TODO add later
        }

        // TODO put in its own function
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