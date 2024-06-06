
#include <set>
#include <random>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>
#include <string.h>
#include <vector>
#include <unordered_set>
#include "Game.h"


Game::Game(Params prm, unsigned int game_number) {
    
    game_num = game_number;

    Distribution::seed(game_num);
    gen.seed(game_num);
    // Uncomment if you want to seed with true randomness
    // std::random_device rd;  
    // gen.seed(rd)
     
    Attacker::reset();
    Defender::reset();
    Insurer::reset();

    p = prm;

    uint num_insurers = p.NUM_INSURERS_distribution->draw();
    for (uint j=0; j < num_insurers; j++) {
        Insurer i = Insurer(j, p);
        insurers.push_back(i);
        alive_insurers_indices.push_back(j);
    }
    Insurer::attackers = &attackers;
    
    Insurer::loss_ratio = p.LOSS_RATIO_distribution->draw();
    Insurer::retention_regression_factor = p.RETENTION_REGRESSION_FACTOR_distribution->draw();

    uint num_blue_players = p.NUM_DEFENDERS_distribution->draw();
    for (uint i=0; i < num_blue_players; i++) {
        Defender d = Defender(i, p);
        defenders.push_back(d);
        alive_defenders_indices.push_back(i);
    }
    Defender::insurers = &insurers;
    Defender::alive_insurers_indices = &alive_insurers_indices;
    Defender::ransom_b0 = p.RANSOM_B0_distribution->mean();
    Defender::ransom_b1 = p.RANSOM_B1_distribution->mean();
    Defender::recovery_base = p.RECOVERY_COST_BASE_distribution->mean();
    Defender::recovery_exp  = p.RECOVERY_COST_EXP_distribution->mean();

    NUM_ATTACKERS = p.NUM_ATTACKERS_distribution->draw();
    Defender::NUM_QUOTES = p.NUM_QUOTES_distribution->draw();
    
    ATTACKS_PER_EPOCH = p.ATTACKS_PER_EPOCH_distribution->draw();
    Insurer::ATTACKS_PER_EPOCH = &ATTACKS_PER_EPOCH;

    Defender::gen = &gen;

    assert(NUM_ATTACKERS > 0);
    assert(ATTACKS_PER_EPOCH > 0);

    Attacker::inequality_ratio = p.INEQUALITY_distribution->draw();
    assert(Attacker::inequality_ratio > 0);
    assert(Attacker::inequality_ratio <= 1.0);

    for (uint32_t i=0; i < NUM_ATTACKERS; i++) {
        Attacker a = Attacker(i, p);
        attackers.push_back(a);
        alive_attackers_indices.push_back(i);
    }
    
    DELTA = (int32_t) p.DELTA_distribution->draw();
    MAX_ITERATIONS = (int32_t) p.MAX_ITERATIONS_distribution->draw();

    if (p.verbose) {
        Defender::cumulative_assets.push_back(Defender::d_init);
        Attacker::cumulative_assets.push_back(Attacker::Attacker::a_init);
        Insurer::cumulative_assets.push_back(Insurer::i_init);

        cumulative_num_alive_defenders.push_back(alive_defenders_indices.size());
        cumulative_num_alive_attackers.push_back(alive_attackers_indices.size());
        cumulative_num_alive_insurers.push_back(alive_insurers_indices.size());
    }

    // Make sure everything got set up correctly
    if (p.assertions_on) {
        verify_init();  
    }   
}

std::string Game::to_string() {
    std::stringstream ss;

    ss << std::scientific << std::setprecision(2) << Defender::d_init <<  ",";
    ss << std::scientific << std::setprecision(2) << Defender::current_sum_assets <<  ",";
    ss << std::scientific << std::setprecision(2) << Attacker::a_init <<  ",";
    ss << std::scientific << std::setprecision(2) << Attacker::current_sum_assets <<  ",";
    ss << std::scientific << std::setprecision(2) << Insurer::i_init <<  ",";
    ss << std::scientific << std::setprecision(2) << Insurer::current_sum_assets <<  ",";
    ss << std::scientific << std::setprecision(2) << Attacker::attacksAttempted <<  ",";
    ss << std::scientific << std::setprecision(2) << Attacker::attacksSucceeded <<  ",";
    ss << std::scientific << std::setprecision(2) << Attacker::attackerLoots <<  ",";
    ss << std::scientific << std::setprecision(2) << Defender::sum_recovery_costs <<  ",";
    ss << std::scientific << std::setprecision(2) << Attacker::attackerExpenditures <<  ",";
    ss << std::scientific << std::setprecision(2) << Defender::policiesPurchased << ",";   
    ss << std::scientific << std::setprecision(2) << Defender::defensesPurchased <<  ",";
    ss << std::scientific << std::setprecision(2) << Defender::sum_security_investments <<  ",";

    ss << Defender::NUM_QUOTES << ",";
    ss << std::scientific << std::setprecision(2) << Insurer::sum_premiums_collected <<  ",";
    ss << std::scientific << std::setprecision(2) << Insurer::paid_claims <<  ",";
    ss << std::scientific << std::setprecision(2) << Insurer::operating_expenses <<  ",";
    ss << iter_num + 1 <<  ",";
    if (final_outcome == Outcomes::ATTACKERS_WIN) {
        ss << "ATTACKERS_WIN" << "";
    } else if (final_outcome == Outcomes::DEFENDERS_WIN) {
        ss << "DEFENDERS_WIN" << "";
    } else if (final_outcome == Outcomes::EQUILIBRIUM) {
        ss << "EQUILIBRIUM" << "";
    } else if (final_outcome == Outcomes::NO_EQUILIBRIUM) {
        ss << "NO_EQUILIBRIUM" << "";
    } else {
        assert(false);
    }

    if (p.verbose) {
        ss << ",\"[";
        for (auto & d : Defender::cumulative_assets) {
            ss << std::scientific << std::setprecision(2) << d;
            if (&d != &Defender::cumulative_assets.back()) {
                ss << ",";
            }
        }
        ss << "]\",";
        ss << "\"[";
        for (auto & a : Attacker::cumulative_assets) {
            ss << std::scientific << std::setprecision(2) << a;
            if (&a != &Attacker::cumulative_assets.back()) {
                ss << ",";
            }
        }
        ss << "]\",";
        ss << "\"[";
        for (auto & i : Insurer::cumulative_assets) {
            ss << std::scientific << std::setprecision(2) << i;
            if (&i != &Insurer::cumulative_assets.back()) {
                ss << ",";
            }
        }
        ss << "]\",";
        ///////////////////////////////////////////////////
        ss << "\"[";
        for (auto & d : cumulative_num_alive_defenders) {
            ss << std::scientific << std::setprecision(2) << d;
            if (&d != &cumulative_num_alive_defenders.back()) {
                ss << ",";
            }
        }
        ss << "]\",";
        ss << "\"[";
        for (auto & a : cumulative_num_alive_attackers) {
            ss << std::scientific << std::setprecision(2) << a;
            if (&a != &cumulative_num_alive_attackers.back()) {
                ss << ",";
            }
        }
        ss << "]\",";
        ss << "\"[";
        for (auto & i : cumulative_num_alive_insurers) {
            ss << std::scientific << std::setprecision(2) << i;
            if (&i != &cumulative_num_alive_insurers.back()) {
                ss << ",";
            }
        }
        ss << "]\"";

    }

    
    ss << "\n";
    return ss.str();
}

void Game::verify_init() {

    assert(MAX_ITERATIONS > 0);
    assert(DELTA > 0);

    for (uint i=0; i<defenders.size(); i++) {
        Defender d = defenders[i];
        assert(d.id >= 0);
        assert(d.id < defenders.size());
        assert(d.assets >= 0);
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

    assert(Defender::current_sum_assets >= 0);
    assert(Attacker::current_sum_assets >= 0);
    assert(Insurer::current_sum_assets  >= 0);

    double checksum_attacker_sum_assets = 0;
    for (uint i=0; i<attackers.size(); i++) {
        assert(attackers[i].assets >= 0);
        checksum_attacker_sum_assets += attackers[i].assets;
    }
    assert(Attacker::current_sum_assets - checksum_attacker_sum_assets == 0); 

    double checksum_defender_sum_assets = 0;
    for (uint i=0; i<defenders.size(); i++) {
        Defender d = defenders[i];
        assert(d.assets >= 0);
        checksum_defender_sum_assets += d.assets;
    }
    assert(Defender::current_sum_assets - checksum_defender_sum_assets == 0);

    double checksum_insurer_sum_assets = 0;
    for (uint i=0; i<insurers.size(); i++) {
        Insurer ins = insurers[i];
        assert(ins.assets >= 0);
        checksum_insurer_sum_assets += ins.assets;
    }
    assert(Insurer::current_sum_assets - checksum_insurer_sum_assets == 0);
    assert(Insurer::paid_claims >= 0);
    assert(Attacker::attackerExpenditures >= 0);

    if (final_outcome == Outcomes::EQUILIBRIUM) {
        assert(alive_attackers_indices.size() > 0);
        assert(alive_defenders_indices.size() > 0);
        assert(iter_num >= DELTA); 
    } else if (final_outcome == Outcomes::ATTACKERS_WIN) {
        assert(alive_defenders_indices.size() == 0);
    } else if (final_outcome == Outcomes::DEFENDERS_WIN) {
        assert(alive_attackers_indices.size() == 0);
    }

    // Master checksum
    int64_t init_ = Defender::d_init + Attacker::Attacker::a_init + Insurer::i_init; 
    int64_t end_  = Defender::current_sum_assets + Attacker::current_sum_assets + Insurer::current_sum_assets;
    int64_t expenses_ = Defender::sum_security_investments +  Attacker::attackerExpenditures + Defender::sum_recovery_costs + Insurer::operating_expenses; 

    assert(init_ - expenses_ == end_); 
}

bool Game::equilibrium_reached() {
    if (roundAttacks == 0) {
        consecutiveNoAttacks++;
    } else {
        consecutiveNoAttacks = 0;
    }

    // TODO think about what value DELTA should be
    return (consecutiveNoAttacks >= DELTA);
}

bool Game::game_over() {

    bool game_over = false;
    if (alive_attackers_indices.size() == 0) {
        final_outcome = Outcomes::DEFENDERS_WIN;
        game_over = true;
    } else if (alive_defenders_indices.size() == 0) {
        final_outcome = Outcomes::ATTACKERS_WIN;
        game_over = true;
    } else if (equilibrium_reached()) {
        final_outcome = Outcomes::EQUILIBRIUM;
        game_over = true;
    } else if (iter_num == MAX_ITERATIONS) {
        final_outcome = Outcomes::NO_EQUILIBRIUM;
        game_over = true;
    }

    return game_over;
}

void Game::fight(Attacker &a, Defender &d) {

    assert(d.attacked == false);
    assert(d.assets > 0);
    assert(a.assets > 0);

    uint32_t ransom = Defender::ransom_cost(d.assets);
    uint32_t recovery_cost = Defender::recovery_cost(d.assets);

    if (ransom > d.assets) {
        ransom = d.assets;  // Mercy kill the defender if the ransom is low
    }

    // Attackers don't know defenders' posture until they attack and cannot compute the odds of success
    // But they can compute the expected ransom payoff

    double estimated_probability_of_attack_success = (1 - Attacker::estimated_current_defender_posture_mean);
    assert(estimated_probability_of_attack_success <= 1);
    assert(estimated_probability_of_attack_success >= 0);

    uint32_t expected_payoff = ransom * estimated_probability_of_attack_success;
    assert(expected_payoff >= 0);

    uint32_t expected_cost_to_attack = (uint32_t) (p.CTA_SCALING_FACTOR_distribution->mean() * Attacker::estimated_current_defender_posture_mean * ransom); 

    if (expected_payoff > expected_cost_to_attack && expected_cost_to_attack <= a.assets) { 
        // Attacking  appears to be financially worth it

        uint32_t cost_to_attack = (uint32_t) (p.CTA_SCALING_FACTOR_distribution->draw() * d.posture * ransom);
        

        // This is to model that attackers can go "all in" but they can't get away with paying less than the full cost of an attack
        uint32_t debt = 0;
        if (cost_to_attack > a.assets) {
            debt = cost_to_attack - a.assets;
            cost_to_attack = a.assets;
        }

        // bookkeeping
        Attacker::attacksAttempted += 1;
        Attacker::attackerExpenditures += cost_to_attack;
        roundAttacks += 1;
        a.lose(cost_to_attack);
        d.attacked = true;

        if (RandUniformDist.draw() > d.posture) {

            Attacker::attacksSucceeded += 1;
            a.gain(ransom);

            if (debt > 0) {
                assert(ransom > debt); 
                // This should always be true. ransom is always greater than cost_to_Attack
                // So the debt is always less than the ransom.
                // If the attacker gains the ransom, they should be able to pay the debt. 
                // Basically I just want to make sure that attackers are not winning ransoms and then the debt is still more than they can pay even with ransom.
                a.lose(debt); // pay off the debt they accrued
                Attacker::attackerExpenditures += debt;
            }

            d.lose(ransom);

            if (recovery_cost > d.assets) {
                recovery_cost = d.assets;
            }
            
            d.lose(recovery_cost);
            Defender::sum_recovery_costs += recovery_cost;
            
            if (d.insured) {
                uint32_t total_losses = ransom + recovery_cost;
                d.submit_claim(total_losses);
            }
        }
    }
}

void Game::init_round() {

    Defender::defender_iter_sum = 0;
    Attacker::attacker_iter_sum = 0;
    Insurer::insurer_iter_sum = 0;
    roundAttacks = 0;

    Insurer::perform_market_analysis(insurers, alive_defenders_indices.size());
    Defender::perform_market_analysis(prevRoundAttacks, alive_defenders_indices.size());
    Attacker::perform_market_analysis(defenders);

    // this could be faster if you iterated through the alive players instead 
    for (Defender &d : defenders) {
        if (d.is_alive()) {            
            // Insurance policy expires
            d.ins_idx = -1;
            d.insured = false; 
            
            // reset attacked status
            d.attacked = false;
            
            d.security_depreciation();
            d.choose_security_strategy(); 
        }
    }
}

void Game::conclude_round() {
    assert(roundAttacks <= alive_defenders_indices.size());
    prevRoundAttacks = roundAttacks;

    // this could be faster if you iterated through the alive players instead 
    alive_defenders_indices.clear();
    for (Defender &d : defenders) {
        if (d.is_alive()) {
            alive_defenders_indices.push_back(d.id);
        }
    }
    
    alive_attackers_indices.clear();
    for (Attacker &a : attackers) {
        if (a.is_alive()) {
            alive_attackers_indices.push_back(a.id);
        }
    }

    alive_insurers_indices.clear();
    for (Insurer &i : insurers) {
        if (i.is_alive()) {
            alive_insurers_indices.push_back(i.id);
        }
    }

    if (p.verbose) {
        Defender::cumulative_assets.push_back(Defender::current_sum_assets);
        Attacker::cumulative_assets.push_back(Attacker::current_sum_assets);
        Insurer::cumulative_assets.push_back(Insurer::current_sum_assets);

        cumulative_num_alive_defenders.push_back(alive_defenders_indices.size());
        cumulative_num_alive_attackers.push_back(alive_attackers_indices.size());
        cumulative_num_alive_insurers.push_back(alive_insurers_indices.size());
    }
}

void Game::init_game(){
    return;
}

void Game::conclude_game(){
    game_over();
    verify_outcome();
}

void Game::run_iterations() {

    init_game();

    for (iter_num = 0; iter_num < MAX_ITERATIONS; iter_num++) {

        init_round();

        std::uniform_int_distribution<> alive_defender_indices_dist(0, alive_defenders_indices.size()-1);

        for (auto& a_i : alive_attackers_indices) {
            assert(a_i < attackers.size());
            Attacker *a = &attackers[a_i];

            
            // pick victims
            std::unordered_set<unsigned int> victim_indices;
            while (victim_indices.size() < std::min(ATTACKS_PER_EPOCH,  (uint32_t) alive_defenders_indices.size())) {
                // This quick-and-dirty approach has the potential to become very slow in degenerate cases
                // Check here if performance becomes an issue
                int candidate_victim = alive_defender_indices_dist(gen);
                victim_indices.insert(candidate_victim);
            }
            // May be more cache friendly if victims are sorted here? May or may not be worth the overhead


            // fight all chosen victims 
            for (const auto& d_i : victim_indices) {
                assert(d_i < defenders.size());
                Defender *d = &defenders[d_i];
                
                if (!d->attacked) {
                    fight(*a, *d);
                }
                
                if (!a->is_alive()) {
                    break;
                }
            }
        }
        conclude_round();
        if (game_over()) {
            break;
        }
    }
    conclude_game();
}

Game::~Game() {
    delete p.NUM_ATTACKERS_distribution;
    delete p.INEQUALITY_distribution;
    delete p.RANSOM_B0_distribution;
    delete p.RANSOM_B1_distribution;
    delete p.RECOVERY_COST_BASE_distribution;
    delete p.RECOVERY_COST_EXP_distribution;   
    delete p.WEALTH_distribution;     
    delete p.POSTURE_distribution;
    delete p.POSTURE_NOISE_distribution;
    delete p.NUM_QUOTES_distribution;
    delete p.LOSS_RATIO_distribution;
    delete p.RETENTION_REGRESSION_FACTOR_distribution;
    delete p.NUM_DEFENDERS_distribution;
    delete p.NUM_INSURERS_distribution;
    delete p.ATTACKS_PER_EPOCH_distribution;
    delete p.CTA_SCALING_FACTOR_distribution;
    delete p.DELTA_distribution;
    delete p.DEPRECIATION_distribution;
    delete p.TARGET_SECURITY_SPENDING_distribution;
    delete p.INVESTMENT_SCALING_FACTOR_distribution;
    delete p.MAX_ITERATIONS_distribution;
}