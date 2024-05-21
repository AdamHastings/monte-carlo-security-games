#pragma once

#include "params.h"
#include "Player.h"

class Attacker : public Player {
    public:
        uint id;

        static double inequality_ratio;
        
        static double a_init;
        static double attacker_iter_sum; // how much the attackers have cumulatively gained or lost this round
        static std::vector<double> cumulative_assets; // running total of all attackers' assets
        static double current_sum_assets; // sum total of all class instances

        static double estimated_current_defender_wealth_mean;
        static double estimated_current_defender_wealth_stdddev;
        static double estimated_current_defender_posture_mean;
        static double estimated_current_defender_posture_stdddev;

        // bookkeeping vars for all attackers combined
        static int attacksAttempted;
        static int attacksSucceeded;
        static double attackerExpenditures;
        static double attackerLoots;


        static void reset();

        Attacker(int id_in,  Params &p);
        void gain(double gain) override;
        void lose(double loss) override;

        // To re-estimate current defender params
        void perform_market_analysis(); // TODO TODO TODO implement
};