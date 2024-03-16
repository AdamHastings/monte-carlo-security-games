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
        Game(Params prm);
        ~Game();
        void run_iterations();
        std::string to_string();

    private:

        Params p;
        std::vector<Defender> defenders;
        std::vector<Attacker> attackers;
        std::vector<Insurer> insurers;

        UniformRealDistribution RandUniformDist = UniformRealDistribution(0.0, 1.0);

        int iter_num;
        std::vector<int> crossovers;
        std::vector<int> insurerTimesOfDeath; // TODO should probably be a class variable

        std::vector<double> last_delta_defenders_changes;
        std::vector<double> last_delta_attackers_changes;

        int outside_epsilon_count_defenders;
        int outside_epsilon_count_attackers;

        std::vector<int> alive_attackers_indices; //(alive_attackers.begin(), alive_attackers.end()); // TODO maybe optimize this later
        std::vector<int> alive_defenders_indices; //(alive_defenders.begin(), alive_defenders.end()); // TODO maybe optimize this later. Try just using a list instead of having to copy to a list each time..?
        
        int roundAttacks = 0;
        int prevRoundAttacks = 0;
        int consecutiveNoAttacks=0;

        std::string final_outcome = "X"; // TODO turn this into an enum perhaps?

        void fight(Attacker &a, Defender &d);
        void conclude_game(std::string outcome);
        bool is_equilibrium_reached();

        void verify_init();
        void verify_outcome();

        void init_round();
        void init_game();

        bool defenders_have_more_than_attackers;


};