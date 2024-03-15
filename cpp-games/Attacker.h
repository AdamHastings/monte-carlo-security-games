#pragma once

#include "params.h"
#include "Player.h"

class Attacker : public Player {
    public:
        uint id;
        
        static double a_init;
        static double attacker_iter_sum; // how much the attackers have cumulatively gained or lost this round
        static std::vector<double> cumulative_assets; // running total of all attackers' assets
        static double current_sum_assets; // sum total of all class instances

        // bookkeeping vars for all attackers combined
        static int attacksAttempted;
        static int attacksSucceeded;
        static double attackerExpenditures;
        static double attackerLoots;

        Attacker(int id_in,  Params &p);
        void gain(double gain) override;
        void lose(double loss) override;
};