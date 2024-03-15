#pragma once

#include "params.h"
#include "Player.h"



class Insurer : public Player {
    public:
        uint id;
        static uint i_init;
        static double insurer_iter_sum;
        static std::vector<double> cumulative_assets; // running total of all insurers' assets
        static double current_sum_assets; // sum total of all class instances

        Insurer(int id_in, Params &p);
        void gain(double gain) override;
        void lose(double loss) override;

        PolicyType provide_a_quote(double assets, double posture, double estimated_costToAttackPercentile);
        double issue_payment(double claim);

        static std::vector<double> attacker_assets;
        static int num_attackers;
        static int num_defenders;
};