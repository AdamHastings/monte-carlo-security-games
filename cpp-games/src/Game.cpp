
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
    // TODO maybe all this should happen in the Insurers' constructor? Or no because it's only called once?
    Insurer::loss_ratio = p.LOSS_RATIO_distribution->draw();
    Insurer::retention_regression_factor = p.RETENTION_REGRESSION_FACTOR_distribution->draw();
    // Insurer::expected_ransom_base = p.RANSOM_BASE_distribution->mean();
    // Insurer::expected_ransom_exponent = p.RANSOM_EXP_distribution->mean();
    // Insurer::expected_recovery_base = p.RECOVERY_COST_BASE_distribution->mean();
    // Insurer::expected_recovery_exponent = p.RECOVERY_COST_EXP_distribution->mean();

    uint num_blue_players = p.NUM_DEFENDERS_distribution->draw();
    for (uint i=0; i < num_blue_players; i++) {
        Defender d = Defender(i, p, insurers);
        defenders.push_back(d);
        alive_defenders_indices.push_back(i);
    }
    Defender::ransom_b0 = p.RANSOM_B0_distribution->mean();
    Defender::ransom_b1 = p.RANSOM_B1_distribution->mean();
    Defender::recovery_base = p.RECOVERY_COST_BASE_distribution->mean();
    Defender::recovery_exp  = p.RECOVERY_COST_EXP_distribution->mean();

    NUM_ATTACKERS = p.NUM_ATTACKERS_distribution->draw();
    Defender::NUM_QUOTES = p.NUM_QUOTES_distribution->draw();
    
    ATTACKS_PER_EPOCH = p.ATTACKS_PER_EPOCH_distribution->draw();
    Insurer::ATTACKS_PER_EPOCH = &ATTACKS_PER_EPOCH;

    cta_scaling_factor = p.CTA_SCALING_FACTOR_distribution->draw();
    Insurer::cta_scaling_factor = &cta_scaling_factor;

    // Insurer::gen = &gen;
    Defender::gen = &gen;

    assert(NUM_ATTACKERS > 0);
    assert(ATTACKS_PER_EPOCH > 0);

    Attacker::inequality_ratio = p.INEQUALITY_distribution->draw();
    assert(Attacker::inequality_ratio > 0);
    assert(Attacker::inequality_ratio <= 1.0);

    for (int i=0; i < NUM_ATTACKERS; i++) {
        Attacker a = Attacker(i, p);
        attackers.push_back(a);
        alive_attackers_indices.push_back(i);
    }
    
    DELTA = p.DELTA_distribution->draw();

    if (p.verbose) {
        Defender::cumulative_assets.push_back(Defender::d_init);
        Attacker::cumulative_assets.push_back(Attacker::Attacker::a_init);
        Insurer::cumulative_assets.push_back(Insurer::i_init);
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
    ss << std::scientific << std::setprecision(2) << Attacker::attackerExpenditures <<  ",";
    ss << std::scientific << std::setprecision(2) << Defender::policiesPurchased << ",";   
    ss << std::scientific << std::setprecision(2) << Defender::defensesPurchased <<  ",";

    ss << Defender::NUM_QUOTES << ",";

    ss << std::scientific << std::setprecision(2) << Insurer::paid_claims <<  ",";
    ss << iter_num + 1 <<  ",";
    ss << final_outcome << "";

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
        ss << "]\"";
    }

    
    ss << "\n";
    return ss.str();
}

void Game::verify_init() {

    // TODO store each D's init conditions? To compare against later...?
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

    // assert(round(Defender::d_init - current_defender_sum_assets) >= 0); // This might actually not be the case! E.g. all defender losses have been covered, and an attacker who received no claims then gets recouped.
    // assert(round(Insurer::i_init  + Insurer- Insurer::paid_claims) >= 0); // This can be violated if insurers collect money from insurance.
    assert(Insurer::paid_claims >= 0);
    // assert(round(Attacker::attackerLoots - Insurer::paid_claims) >= 0); // No longer always true in new model because of recovery costs
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
    long long init_ = Defender::d_init + Attacker::Attacker::a_init + Insurer::i_init; 
    long long end_  = Defender::current_sum_assets + Attacker::current_sum_assets + Insurer::current_sum_assets;
    long long expenses_ = Defender::sum_security_investments +  Attacker::attackerExpenditures + Defender::sum_recovery_costs + Insurer::operating_expenses; 

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
        final_outcome = "A"; // TODO make this an enum
        game_over = true;
    } else if (alive_defenders_indices.size() == 0) {
        final_outcome = "D";
        game_over = true;
    } else if (equilibrium_reached()) {
        final_outcome = "E";
        game_over = true;
    } else if (iter_num == max_iterations) {
        final_outcome = "N";
        game_over = true;
    }

    return game_over;
}

void Game::fight(Attacker &a, Defender &d) {

    if (a.assets == 0) {
        // One player is dead already. Skip.
        return;
    }
    if (d.assets == 0) {
        // One player is dead already. Skip.
        return;
    }

    uint32_t ransom = Defender::ransom(d.assets);
    uint32_t recovery_cost = Defender::recovery_cost(d.assets);

    if (ransom > d.assets) {
        // Mercy kill the defender if the ransom is low
        ransom = d.assets;
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

        uint32_t cost_to_attack = (uint32_t) (p.CTA_SCALING_FACTOR_distribution->mean() * d.posture * ransom);
        

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

        // verify_outcome(); // TODO delete

        if (RandUniformDist.draw() > d.posture) {

            Attacker::attacksSucceeded += 1;
            // roundAttackSuccesses += 1;
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
            // verify_outcome(); // TODO delete
        }
        // verify_outcome(); // TODO delete
    }
    // verify_outcome(); // TODO delete
}

void Game::init_round() {

    Defender::defender_iter_sum = 0;
    Attacker::attacker_iter_sum = 0;
    Insurer::insurer_iter_sum = 0;
    roundAttacks = 0;
    // roundAttackSuccesses = 0;

    Insurer::perform_market_analysis(insurers);
    Defender::perform_market_analysis(prevRoundAttacks, defenders.size());
    Attacker::perform_market_analysis(defenders);

    // this could be faster if you iterated through the alive players instead 
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

    alive_defenders_indices.clear();
    for (Defender &d : defenders) {
        if (d.is_alive()) {
            alive_defenders_indices.push_back(d.id);
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

    prevRoundAttacks = roundAttacks;

    if (p.verbose) {
        Defender::cumulative_assets.push_back(Defender::current_sum_assets);
        Attacker::cumulative_assets.push_back(Attacker::current_sum_assets);
        Insurer::cumulative_assets.push_back(Insurer::current_sum_assets);
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

    for (iter_num = 0; iter_num < max_iterations; iter_num++) {

        init_round();

        unsigned int num_alive_defenders = alive_defenders_indices.size();
        std::uniform_int_distribution<> alive_defender_indices_dist(0, num_alive_defenders-1);

        for (auto& a_i : alive_attackers_indices) {
            
            // pick victims
            // TODO think about how to do this in a cache-friendly way
            std::unordered_set<unsigned int> victim_indices;
            while (victim_indices.size() < std::min(ATTACKS_PER_EPOCH, num_alive_defenders)) {
                // TODO consider having attackers fight until they've attempted ATTACKS_PER_EPOCH attacks
                // TODO potential to get stuck here
                int candidate_victim = alive_defender_indices_dist(gen);
                bool attacked_this_round = defenders[alive_defenders_indices[candidate_victim]].attacked;
                if (!attacked_this_round) {
                    victim_indices.insert(candidate_victim);
                }
            }

            // fight all chosen victims 
            for (const auto& d_i : victim_indices) {
                Attacker *a = &attackers[alive_attackers_indices[a_i]];
                Defender *d = &defenders[alive_defenders_indices[d_i]];
                fight(*a, *d);
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
    // delete p.EFFICIENCY_distribution; 
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
}