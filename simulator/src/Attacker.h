#pragma once

#include "params.h"
#include "Player.h"

class Attacker : public Player {
    public:
        uint64_t id;

        static double inequality_ratio;
        
        static long long  a_init;
        static int64_t  attacker_iter_sum; // how much the attackers have cumulatively gained or lost this round
        static std::vector<unsigned long long> cumulative_assets; // running total of all attackers' assets
        static int64_t current_sum_assets; // sum total of all class instances

        static double estimated_current_defender_posture_mean;

        // bookkeeping vars for all attackers combined
        static long long  attacksAttempted;
        static long long  attacksSucceeded;
        static long long  attackerExpenditures;
        static long long  attackerLoots;


        static void reset();

        Attacker(int id_in,  Params &p);
        void gain(int64_t gain) override;
        void lose(int64_t loss) override;

        // To re-estimate current defender params
        static void perform_market_analysis(std::vector<Defender> &defenders); 
};