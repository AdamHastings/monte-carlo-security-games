#pragma once

#include "params.h"
#include "Player.h"
#include "Attacker.h"
#include "Defender.h"



class Insurer : public Player {
    public:
        static unsigned long long i_init;
        static int64_t insurer_iter_sum; // Can be negative! Must be signed. 
        static std::vector<unsigned long long> cumulative_assets; // running total of all insurers' assets
        static int64_t current_sum_assets; // sum total of all class instances
        
        static double estimated_current_attacker_wealth_mean;
        static double estimated_current_attacker_wealth_stdddev;

        static double loss_ratio;
        static unsigned int* ATTACKS_PER_EPOCH;
        static double* cta_scaling_factor;
        static std::mt19937* gen;

        static double retention_regression_factor;  // TODO double check retention regression factor


        static unsigned long long paid_claims;
        static unsigned long long operating_expenses;

        static std::vector<Defender>* defenders;
        static std::vector<Attacker>* attackers;

        static void reset();

    public:
        uint64_t id;
        // uint64_t round_earnings = 0; // TODO make sure these are updated in gain
        int64_t round_losses = 0; // TODO make sure these are updated in loss

        Insurer(int id_in, Params &p, std::vector<Defender>& _defenders, std::vector<Attacker>& _attackers);
        void gain(int64_t gain) override;
        void lose(int64_t loss) override;

        PolicyType provide_a_quote(int64_t assets, double posture);
        int64_t issue_payment(int64_t claim);
        static void perform_market_analysis(std::vector<Insurer> &insurers);
};