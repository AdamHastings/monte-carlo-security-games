#pragma once

#include "params.h"
#include "Player.h"
#include "Attacker.h"
#include "Defender.h"
#include "Game.h"


class Insurer : public Player {
    public:
        static unsigned long long i_init;
        static int64_t insurer_iter_sum; // Can be negative! Must be signed. 
        static std::vector<unsigned long long> cumulative_assets; // running total of all insurers' assets
        static int64_t current_sum_assets; // sum total of all class instances
        
        static double estimated_current_attacker_wealth_mu;
        static double estimated_current_attacker_wealth_sigma;

        static double loss_ratio;
        static unsigned int* ATTACKS_PER_EPOCH;

        static double retention_regression_factor;

        static unsigned long long paid_claims;
        static unsigned long long operating_expenses;
        static int64_t sum_premiums_collected;

        static double p_attack;

        static std::vector<Attacker>* attackers;

        static void reset();

    public:
        uint64_t id;
        int64_t round_losses = 0;

        Insurer(int id_in, Params &p);
        void gain(int64_t gain) override;
        void lose(int64_t loss) override;

        PolicyType provide_a_quote(int64_t assets, double posture);
        int64_t issue_payment(int64_t claim);
        void sell_policy(PolicyType policy);
        
        static void perform_market_analysis(std::vector<Insurer> &insurers, int current_num_defenders);
};