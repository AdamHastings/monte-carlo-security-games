
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
    Insurer::expected_ransom_base = p.RANSOM_BASE_distribution->mean();
    Insurer::expected_ransom_exponent = p.RANSOM_EXP_distribution->mean();
    Insurer::expected_recovery_base = p.RECOVERY_COST_BASE_distribution->mean();
    Insurer::expected_recovery_exponent = p.RECOVERY_COST_EXP_distribution->mean();

    uint num_blue_players = p.NUM_DEFENDERS_distribution->draw();
    for (uint i=0; i < num_blue_players; i++) {
        Defender d = Defender(i, p, insurers);
        defenders.push_back(d);
        alive_defenders_indices.push_back(i);
    }

    NUM_ATTACKERS = p.NUM_ATTACKERS_distribution->draw();
    
    ATTACKS_PER_EPOCH = p.ATTACKS_PER_EPOCH_distribution->draw();
    Insurer::ATTACKS_PER_EPOCH = &ATTACKS_PER_EPOCH;

    cta_scaling_factor = p.CTA_SCALING_FACTOR_distribution->draw();
    Insurer::cta_scaling_factor = &cta_scaling_factor;

    Insurer::gen = &gen;

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
    EPSILON = p.EPSILON_distribution->draw();

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
    ss << std::scientific << std::setprecision(2) << Insurer::paid_claims <<  ",";
    ss << iter_num <<  ",";
    ss << final_outcome << ",";

    if (p.verbose) {
        ss << "\"[";
        for (auto d : Defender::cumulative_assets) {
            ss << std::scientific << std::setprecision(2) << d << ",";
        }
        ss << "]\",";
        ss << "\"[";
        for (auto a : Attacker::cumulative_assets) {
            ss << std::scientific << std::setprecision(2) << a << ",";
        }
        ss << "]\",";
        ss << "\"[";
        for (auto i : Insurer::cumulative_assets) {
            ss << std::scientific << std::setprecision(2) << i << ",";
        }
        ss << "]\",";
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
        // assert(d.costToAttack >= 0); // Getting rid of this param
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
    double init_ = Defender::d_init + Attacker::Attacker::a_init + Insurer::i_init; 
    double end_  = Defender::current_sum_assets + Attacker::current_sum_assets + Insurer::current_sum_assets + Attacker::attackerExpenditures + Defender::sum_recovery_costs; 

    assert(round(init_ - end_) == 0); 
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

    verify_outcome(); // TODO delete

    if (a.assets == 0 || d.assets == 0) {
        // One player is dead already. Skip.
        std::cout << "dead player" << std::endl;
        return;
    }

    double ransom = p.RANSOM_BASE_distribution->draw() * pow(d.assets,  p.RANSOM_EXP_distribution->draw());
    double recovery_cost = p.RECOVERY_COST_BASE_distribution->draw() * pow(d.assets, p.RECOVERY_COST_EXP_distribution->draw());

    if (ransom > d.assets) {
        // Mercy kill the defender if the ransom is low
        ransom = d.assets;
    }

    // TODO is (expected_loot > d.costToAttack) part of the underwriting process at the moment? I don't think it is
    // if (ransom > d.costToAttack && d.costToAttack <= a.assets) {
    // Attackers don't know defenders' posture until they attack and cannot compute the odds of success
    // But they can compute the expected ransom

    // TODO maybe attackers should also do a "market analysis" at the start of each round?
    // E.g. estimate params for current wealth and posture distributions 
    // instead of relying on means as is implemented below.
    double estimated_probability_of_attack_success = (1 - p.POSTURE_distribution->mean());
    assert(estimated_probability_of_attack_success <= 1);
    assert(estimated_probability_of_attack_success >= 0);

    double expected_payoff = ransom * estimated_probability_of_attack_success;
    assert(expected_payoff >= 0);

    double expected_cost_to_attack = p.CTA_SCALING_FACTOR_distribution->mean() * p.POSTURE_distribution->mean() * p.WEALTH_distribution->mean();

    if (expected_payoff > expected_cost_to_attack) { 
        // Attacking  appears to be financially worth it

        double cost_to_attack =  p.CTA_SCALING_FACTOR_distribution->mean() * d.posture * d.assets;
        
        // Attackers do a hail mary if it turns out they don't have enough to attack
        // So that we don't end the game with a bunch of attackers with $0.01
        // TODO this might give the attackers a slight advantage...maybe have it count against their earnings if they're succesful?
        // bool cost_gt_assets = false;
        // double debt = 0;
        if (cost_to_attack > a.assets) {
            // debt = cost_to_attack - a.assets;
            // cost_gt_assets = true;
            cost_to_attack = a.assets;
        }

        // bookkeeping
        Attacker::attacksAttempted += 1;
        roundAttacks += 1;
        a.lose(cost_to_attack);
        Attacker::attackerExpenditures += cost_to_attack;

        if (RandUniformDist.draw() > d.posture) {

            Attacker::attacksSucceeded += 1;
            
            a.gain(ransom);
            d.lose(ransom);

            if (recovery_cost > d.assets) {
                recovery_cost = d.assets;
            }
            
            d.lose(recovery_cost);
            Defender::sum_recovery_costs += recovery_cost;

            
            if (d.insured) {
                double total_losses = ransom + recovery_cost;
                d.submit_claim(total_losses);
                verify_outcome(); // TODO delete
            }
            verify_outcome(); // TODO delete
        }
        verify_outcome(); // TODO delete
    }
    verify_outcome(); // TODO delete
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

void Game::conclude_round() {
    reset_alive_players();

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
}

void Game::init_game(){
    return;
}

void Game::conclude_game(){
    game_over();
    verify_outcome();
}

void Game::reset_alive_players() {
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

    alive_insurers_indices.clear();
    for (auto i : insurers) {
        if (std::round(i.assets) > 0) {
            alive_insurers_indices.push_back(i.id);
        }
    }
}

void Game::run_iterations() {

    init_game();


    for (iter_num = 1; iter_num < max_iterations + 1; iter_num++) {

        verify_outcome(); // TODO delete
        init_round();
        verify_outcome(); // TODO delete

        unsigned int num_alive_defenders = alive_defenders_indices.size();
        std::uniform_int_distribution<> alive_defender_indices_dist(0, num_alive_defenders-1);

        for (auto& a_i : alive_attackers_indices) {
            
            // pick victims
            std::unordered_set<unsigned int> victim_indices;
            while (victim_indices.size() < std::min(ATTACKS_PER_EPOCH, num_alive_defenders)) {
                // TODO consider having attackers fight until they've attempted ATTACKS_PER_EPOCH attacks
                victim_indices.insert(alive_defender_indices_dist(gen));
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
    delete p.EFFICIENCY_distribution; 
    delete p.RANSOM_BASE_distribution;
    delete p.RANSOM_EXP_distribution;  
    delete p.RECOVERY_COST_BASE_distribution;
    delete p.RECOVERY_COST_EXP_distribution;   
    delete p.WEALTH_distribution;     
    delete p.POSTURE_distribution;  
    delete p.LOSS_RATIO_distribution;
    delete p.RETENTION_REGRESSION_FACTOR_distribution;
    delete p.NUM_DEFENDERS_distribution;
    delete p.NUM_INSURERS_distribution;
    delete p.ATTACKS_PER_EPOCH_distribution;
    delete p.CTA_SCALING_FACTOR_distribution;
    delete p.EPSILON_distribution;
    delete p.DELTA_distribution;
}