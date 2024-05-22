#pragma once

#include "params.h"
#include "Player.h"
#include "Attacker.h"
#include "Defender.h"



class Insurer : public Player {
    public:
        static unsigned long long i_init;
        static unsigned long long insurer_iter_sum;
        static std::vector<unsigned long long> cumulative_assets; // running total of all insurers' assets
        static unsigned long long current_sum_assets; // sum total of all class instances
        
        static double estimated_current_attacker_wealth_mean;
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


        static unsigned long long paid_claims;

        static std::vector<Defender>* defenders;
        static std::vector<Attacker>* attackers;

        static void reset();

    public:
        uint id;
        double last_round_loss_ratio;

        Insurer(int id_in, Params &p, std::vector<Defender>& _defenders, std::vector<Attacker>& _attackers);
        void gain(uint32_t gain) override;
        void lose(uint32_t loss) override;

        PolicyType provide_a_quote(uint32_t assets, double posture);
        uint32_t issue_payment(uint32_t claim);
        static void perform_market_analysis();

    // private:
        // static double findPercentile(const std::vector<double>& sortedVector, double newValue);

};