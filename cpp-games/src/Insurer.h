#pragma once

#include "params.h"
#include "Player.h"
#include "Attacker.h"
#include "Defender.h"



class Insurer : public Player {
    public:
        static double i_init;
        static double insurer_iter_sum;
        static std::vector<double> cumulative_assets; // running total of all insurers' assets
        static double current_sum_assets; // sum total of all class instances
        
        static double estimated_current_attacker_welth_mean;
        static double estimated_current_attacker_wealth_stdddev;

        static double loss_ratio;
        static unsigned int* ATTACKS_PER_EPOCH;
        static double* cta_scaling_factor;
        static std::mt19937* gen;

        static double retention_regression_factor;  // TODO double check retention regression factor
        static double expected_ransom_base;
        static double expected_ransom_exponent;
        static double expected_recovery_base;
        static double expected_recovery_exponent;


        static double paid_claims;

        static std::vector<Defender>* defenders;
        static std::vector<Attacker>* attackers;

        static void reset();

    public:
        uint id;
        double last_round_loss_ratio;

        Insurer(int id_in, Params &p, std::vector<Defender>& _defenders, std::vector<Attacker>& _attackers);
        void gain(double gain) override;
        void lose(double loss) override;

        PolicyType provide_a_quote(double assets, double posture);
        double issue_payment(double claim);
        static void perform_market_analysis(int prevRoundAttacks);

    // private:
        // static double findPercentile(const std::vector<double>& sortedVector, double newValue);

};