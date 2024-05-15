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


class Game {

    public:
        Game(Params prm, unsigned int game_number);
        ~Game();
        void run_iterations();
        std::string to_string();

    private:

        unsigned int num_games = 0;
        unsigned int game_num = 0;

        Params p;

        std::mt19937 gen;
        double INEQUALITY;
        int NUM_ATTACKERS;
        unsigned int ATTACKS_PER_EPOCH; 


        uint DELTA;
        uint EPSILON;

        std::vector<Defender> defenders;
        std::vector<Attacker> attackers;
        std::vector<Insurer> insurers;

        UniformRealDistribution RandUniformDist = UniformRealDistribution(0.0, 1.0);

        unsigned int iter_num = 0;

        std::vector<int> alive_attackers_indices;
        std::vector<int> alive_defenders_indices;
        
        uint roundAttacks = 0;
        uint prevRoundAttacks = 0;
        uint consecutiveNoAttacks=0;

        std::string final_outcome = "X"; // TODO turn this into an enum perhaps?

        void fight(Attacker &a, Defender &d);
        void conclude_game(std::string outcome);
        bool equilibrium_reached();
        bool game_over();

        void verify_init();
        void verify_outcome();

        void init_round();
        void init_game();
};