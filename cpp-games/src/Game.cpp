
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
#include <unordered_set>
#include "Game.h"


Game::Game(Params prm, unsigned int game_number) {
    
    game_num = game_number;

    // std::random_device rd;  // Uncomment if you need true randomness
    Distribution::seed(game_num);
    gen.seed(game_num);

     
    Attacker::reset();
    Defender::reset();
    Insurer::reset();

    p = prm;

    uint num_insurers = p.NUM_INSURERS_distribution->draw();
    for (uint j=0; j < num_insurers; j++) {
        Insurer i = Insurer(j, p, defenders, attackers);
        insurers.push_back(i);
    }
    Insurer::loss_ratio = p.LOSS_RATIO_distribution->draw();
    Insurer::retention_regression_factor = p.RETENTION_REGRESSION_FACTOR_distribution->draw();

    uint num_blue_players = p.NUM_DEFENDERS_distribution->draw();
    for (uint i=0; i < num_blue_players; i++) {
        Defender d = Defender(i, p, insurers);
        defenders.push_back(d);
        alive_defenders_indices.push_back(i);
    }

    NUM_ATTACKERS = p.NUM_ATTACKERS_distribution->draw();
    INEQUALITY = p.INEQUALITY_distribution->draw();

    int num_attackers = (int)(NUM_ATTACKERS);
    if (num_attackers <= 0) {
        num_attackers = 1;
    }
    assert(num_attackers > 0);
    for (int i=0; i < num_attackers; i++) { // TODO isn't this wrong now because p.ATTACKERS is a distribution?
        Attacker a = Attacker(i, p, INEQUALITY);
        attackers.push_back(a);
        alive_attackers_indices.push_back(i);
    }
    
    DELTA = p.DELTA_distribution->draw();
    EPSILON = p.EPSILON_distribution->draw();

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

    ret += std::to_string(NUM_ATTACKERS).substr(0,4) + ",";
    ret += std::to_string(INEQUALITY).substr(0,5) + ",";
    ret += std::to_string((long long)(round(Defender::d_init))) + ",";
    ret += std::to_string((long long)(round(Defender::current_sum_assets))) + ",";
    ret += std::to_string((long long)(round(Attacker::Attacker::a_init))) + ",";
    ret += std::to_string((long long)(round(Attacker::current_sum_assets))) + ",";
    ret += std::to_string((long long)(round(Insurer::i_init))) + ",";
    ret += std::to_string((long long)(round(Insurer::current_sum_assets))) + ",";
    ret += std::to_string((long long)(round(Attacker::attacksAttempted))) + ",";
    ret += std::to_string((long long)(round(Attacker::attacksSucceeded))) + ",";
    ret += std::to_string((long long)(round(Attacker::attackerLoots))) + ",";
    ret += std::to_string((long long)(round(Attacker::attackerExpenditures))) + ",";
    ret += std::to_string(Defender::policiesPurchased) + ",";   
    ret += std::to_string(Defender::defensesPurchased) + ",";
    // ret += "\"[";
    // for (auto i : crossovers) {
    //     ret += std::to_string(i) + ",";
    // }
    // ret += "]\",";
    // ret += "\"[";
    // for (auto i : insurerTimesOfDeath) {
    //     ret += std::to_string(i) + ",";
    // }
    // ret += "]\",";
    ret += std::to_string(int(round(Insurer::paid_claims))) + ",";
    ret += std::to_string(iter_num) + ",";
    ret += final_outcome + ",";

    if (p.verbose) {
        ret += "\"[";
        for (auto a : Defender::cumulative_assets) {
            ret += std::to_string(int(round(a))) + ",";
        }
        ret += "]\",";
        ret += "\"[";
        for (auto a : Attacker::cumulative_assets) {
            ret += std::to_string(int(round(a))) + ",";
        }
        ret += "]\",";
        ret += "\"[";
        for (auto a : Insurer::cumulative_assets) {
            ret += std::to_string(int(round(a))) + ",";
        }
        ret += "]\",";
    }
    
    ret += "\n";
    return ret;

}

void Game::verify_init() {

    // TODO store each D's init conditions? To compare against later...?
    for (uint i=0; i<defenders.size(); i++) {
        Defender d = defenders[i];
        assert(d.id >= 0);
        assert(d.id < defenders.size());
        assert(d.assets >= 0);
        assert(d.costToAttack >= 0);
        assert(d.posture >= 0);
        assert(d.posture <= 1);
        assert(d.id == i);
    }
    assert(Defender::d_init == Defender::current_sum_assets);

    for (uint i=0; i<attackers.size(); i++) {
        Attacker a = attackers[i];
        assert(a.id >= 0);
        assert(a.id < attackers.size());
        assert(a.assets >= 0);
        assert(a.id == i);
    }
    assert(Attacker::a_init == Attacker::current_sum_assets);

    for (uint i=0; i<insurers.size(); i++) {
        Insurer ins = insurers[i];
        assert(ins.id >= 0);
        assert(ins.id < insurers.size());
        assert(ins.assets >= 0);
        assert(ins.id == i);
    }
}

void Game::verify_outcome() {

    assert(round(Defender::current_sum_assets) >= 0);
    assert(round(Attacker::current_sum_assets) >= 0);
    assert(round(Insurer::current_sum_assets)  >= 0); 

    double checksum_attacker_sum_assets = 0;
    for (uint i=0; i<attackers.size(); i++) {
        assert(round(attackers[i].assets) >= 0);
        checksum_attacker_sum_assets += attackers[i].assets;
    }
    assert(round(Attacker::current_sum_assets - checksum_attacker_sum_assets) == 0);

    double checksum_defender_sum_assets = 0;
    for (uint i=0; i<defenders.size(); i++) {
        Defender d = defenders[i];
        assert(round(d.assets) >= 0);
        checksum_defender_sum_assets += d.assets;
    }
    assert(round(Defender::current_sum_assets - checksum_defender_sum_assets) == 0);

    double checksum_insurer_sum_assets = 0;
    for (uint i=0; i<insurers.size(); i++) {
        Insurer ins = insurers[i];
        assert(round(ins.assets) >= 0);
        checksum_insurer_sum_assets += ins.assets;
    }
    assert(round(Insurer::current_sum_assets - checksum_insurer_sum_assets) == 0);

    // assert(round(Defender::d_init - current_defender_sum_assets) >= 0); // This might actually not be the case! E.g. all defender losses have been covered, and an attacker who received no claims then gets recouped.
    // assert(round(Insurer::i_init  + Insurer- Insurer::paid_claims) >= 0); // This can be violated if insurers collect money from insurance.
    assert(round(Insurer::paid_claims) >= 0);
    assert(round(Attacker::attackerLoots - Insurer::paid_claims) >= 0);
    assert(Attacker::attackerExpenditures >= 0);

    if (final_outcome == "E") {
        assert(alive_attackers_indices.size() > 0);
        assert(alive_defenders_indices.size() > 0);
        assert(iter_num >= DELTA); 
    } else if (final_outcome == "D") {
        assert(alive_defenders_indices.size() == 0);
    } else if (final_outcome == "A") {
        assert(alive_attackers_indices.size() == 0);
    }

    // Master checksum
    double init_ = Defender::d_init + Attacker::Attacker::a_init + Insurer::i_init; 
    double end_  = Defender::current_sum_assets + Attacker::current_sum_assets + Insurer::current_sum_assets + Attacker::attackerExpenditures; 

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

    return (consecutiveNoAttacks >= DELTA);
}

void Game::fight(Attacker &a, Defender &d) {

    double expected_loot = d.assets * p.PAYOFF_distribution->mean();

    // Mercy kill the Defenders if the loot is very low
    if (d.assets < EPSILON) {
        expected_loot = d.assets;
    }
    
    // TODO should attackers YOLO their savings if their assets get very low?
    // So that we don't end the game with a bunch of attackers with $0.01

    // TODO is (expected_loot > d.costToAttack) part of the underwriting process at the moment? I don't think it is
    if (expected_loot > d.costToAttack && d.costToAttack <= a.assets) {
        // Attacking is financially worth it


        // bookkeeping
        Attacker::attacksAttempted += 1;
        roundAttacks += 1;
        a.lose(d.costToAttack);
        Attacker::attackerExpenditures += d.costToAttack;

        if (RandUniformDist.draw() > d.posture) {

            Attacker::attacksSucceeded += 1;

            double loot = d.assets * p.PAYOFF_distribution->draw();
            // mercy kill
            if (d.assets < EPSILON) {
                loot = d.assets;
            }
            
            a.gain(loot);
            d.lose(loot);


            if (d.insured) {
                d.submit_claim(loot);
            }
        }
    } 
}

void Game::init_round() {

    Defender::defender_iter_sum = 0;
    Attacker::attacker_iter_sum = 0;
    Insurer::insurer_iter_sum = 0;
    roundAttacks = 0;

    Insurer::perform_market_analysis(prevRoundAttacks);
    for (uint i=0; i < alive_defenders_indices.size(); i++) {
        assert(defenders[alive_defenders_indices[i]].assets > 0);
        defenders[alive_defenders_indices[i]].choose_security_strategy();
    }
}

void Game::init_game(){
    return;
}

void Game::run_iterations() {

    init_game();


    for (iter_num = 1; iter_num < num_games + 1; iter_num++) {

        init_round();

        std::uniform_int_distribution<> alive_defender_indices_dist(0, alive_defenders_indices.size());

        for (auto& a_i : alive_attackers_indices) {
            std::unordered_set<unsigned int> victim_indices;
            while (victim_indices.size() < ATTACKS_PER_EPOCH) {
                victim_indices.insert(alive_defender_indices_dist(gen));
            }

            for (const auto& d_i : victim_indices) {
                Attacker *a = &attackers[alive_attackers_indices[a_i]];
                Defender *d = &defenders[alive_defenders_indices[d_i]];
                fight(*a, *d);
            }
        }

        alive_attackers_indices.clear();
        for (auto& a : attackers) {
            if (std::round(a.assets) > 0) {
                alive_attackers_indices.push_back(a.id);
            }
        }

        alive_defenders_indices.clear();
        for (auto d : defenders) {
            if (std::round(d.assets) > 0) {
                alive_defenders_indices.push_back(d.id);
            }
        }

        // Insurance policy expires
        for (uint i=0; i<defenders.size(); i++) {
            Defender d = defenders[i];
            d.ins_idx = -1;
            d.insured = false; 
        }

        prevRoundAttacks = roundAttacks;


        if (p.verbose) {
            Defender::cumulative_assets.push_back(Defender::current_sum_assets);
            Attacker::cumulative_assets.push_back(Attacker::current_sum_assets);
            Insurer::cumulative_assets.push_back(Insurer::current_sum_assets);
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


Game::~Game() {
    delete p.NUM_ATTACKERS_distribution;
    delete p.INEQUALITY_distribution;
    delete p.EFFICIENCY_distribution; 
    delete p.PAYOFF_distribution;     
    delete p.WEALTH_distribution;     
    delete p.POSTURE_distribution;  

    delete p.LOSS_RATIO_distribution;
    delete p.RETENTION_REGRESSION_FACTOR_distribution;
    delete p.NUM_DEFENDERS_distribution;
    delete p.NUM_INSURERS_distribution;
    delete p.EPSILON_distribution;
    delete p.DELTA_distribution;
}