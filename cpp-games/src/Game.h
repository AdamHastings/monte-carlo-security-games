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
        int32_t MAX_ITERATIONS = -1;

        int32_t DELTA;
        int32_t consecutiveNoAttacks=0;


        std::vector<Defender> defenders;
        std::vector<Attacker> attackers;
        std::vector<Insurer> insurers;

        UniformRealDistribution RandUniformDist = UniformRealDistribution(0.0, 1.0);


        std::vector<uint32_t> alive_attackers_indices;
        std::vector<uint32_t> alive_defenders_indices;
        std::vector<uint32_t> alive_insurers_indices;
    

        Outcomes final_outcome = Outcomes::INIT;

    private:
        std::vector<int> cumulative_num_alive_defenders;
        std::vector<int> cumulative_num_alive_attackers;
        std::vector<int> cumulative_num_alive_insurers;

        uint32_t round_pairings = 0;
        uint32_t round_attacks = 0;
        uint32_t prev_round_attacks = 0;
        uint32_t round_loots = 0;

        double p_paired = 0;
        double p_attacked = 0.59; // Give defenders an initial non-zero guess // TODO justify this hard-coded number
        double p_looted = 0;
        std::vector<float> cumulative_p_pairing;
        std::vector<float> cumulative_p_attacked;
        std::vector<float> cumulative_p_looted;
        // std::vector<float> cumulative_insurer_estimate_p_pairing;
        // std::vector<float> cumulative_defender_estimate_p_attack;
        std::vector<float> cumulative_defender_avg_posture;


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

    private:
        std::string get_sweepval(std::string sweepvar);

};