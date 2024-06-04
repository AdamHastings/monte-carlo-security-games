#pragma once

#include <sstream>
#include <vector>
#include <set>
#include "Player.h"
#include "Insurer.h"
#include "Defender.h"
#include "Attacker.h"
#include "params.h"
#include "Distributions.h"

enum class Outcomes {
    INIT,
    DEFENDERS_WIN, 
    ATTACKERS_WIN, 
    EQUILIBRIUM, 
    NO_EQUILIBRIUM
}; 

class Game {

    public:
        Game(Params prm, unsigned int game_number);
        ~Game();
        void run_iterations();
        std::string to_string();

    private:

        uint32_t game_num = 0;

        Params p;

        std::mt19937 gen;
        uint32_t NUM_ATTACKERS; // TODO pick a capitalization convention
        uint32_t ATTACKS_PER_EPOCH; 
        double cta_scaling_factor;
        
        uint32_t max_iterations = 5000; // TODO make this a game parameter

        uint32_t DELTA;

        std::vector<Defender> defenders;
        std::vector<Attacker> attackers;
        std::vector<Insurer> insurers;

        UniformRealDistribution RandUniformDist = UniformRealDistribution(0.0, 1.0);

        uint32_t iter_num = 0;

        std::vector<uint32_t> alive_attackers_indices;
        std::vector<uint32_t> alive_defenders_indices;
        std::vector<uint32_t> alive_insurers_indices;
        
        uint32_t roundAttacks = 0;
        // uint roundAttackSuccesses = 0;
        uint32_t prevRoundAttacks = 0;
        uint32_t consecutiveNoAttacks=0;

        Outcomes final_outcome = Outcomes::INIT;

        void fight(Attacker &a, Defender &d);
        bool equilibrium_reached();
        bool game_over();

        void verify_init();
        void verify_outcome();

        void init_game();
        void conclude_game();

        void init_round();
        void conclude_round();
};