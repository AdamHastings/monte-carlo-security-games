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
        uint32_t NUM_ATTACKERS;
        uint32_t ATTACKS_PER_EPOCH; 

        int32_t iter_num;
        int32_t MAX_ITERATIONS = -1; // TODO make this a game parameter // TODO need to set this in Game construction

        int32_t DELTA;
        int32_t consecutiveNoAttacks=0;


        std::vector<Defender> defenders;
        std::vector<Attacker> attackers;
        std::vector<Insurer> insurers;

        UniformRealDistribution RandUniformDist = UniformRealDistribution(0.0, 1.0);


        std::vector<uint32_t> alive_attackers_indices;
        std::vector<uint32_t> alive_defenders_indices;
        std::vector<uint32_t> alive_insurers_indices;
        
        uint32_t roundAttacks = 0;
        uint32_t prevRoundAttacks = 0;

        Outcomes final_outcome = Outcomes::INIT;

    private:
        std::vector<int> cumulative_num_alive_defenders;
        std::vector<int> cumulative_num_alive_attackers;
        std::vector<int> cumulative_num_alive_insurers;


    private:

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