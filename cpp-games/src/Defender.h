#pragma once 

#include "params.h"
#include "Player.h"
#include "Insurer.h"


class Defender : public Player {

    // static class variables
    public:
        // TODO this is perhaps better called estimated_p_loot
        static double estimated_p_loot;

        static int64_t d_init;
        static int64_t defender_iter_sum;
        static int64_t current_sum_assets; // sum total of all class instances
        static int64_t sum_recovery_costs;
        static int64_t policiesPurchased;
        static int64_t defensesPurchased;
        static int64_t do_nothing; // keep track of the times that the player chooses to make an "investment" of 0, i.e. do nothing
        static int64_t sum_security_investments;
        
        static std::mt19937* gen;

        static double ransom_b0;
        static double ransom_b1;
        static double recovery_base;
        static double recovery_exp;

        static uint32_t NUM_QUOTES;
        static double MANDATORY_INVESTMENT;

        // verbose bookkeeping variables 
        static std::vector<unsigned long long> cumulative_assets; // running total of all defenders' assets
        static std::vector<int> cumulative_round_policies_purchased;
        static std::vector<int> cumulative_round_defenses_purchased;
        static std::vector<int> cumulative_round_do_nothing;
        
        static int round_policies_purchased;
        static int round_defenses_purchased;
        static int round_do_nothing;

    // static class functions
    public:
        static void perform_market_analysis(std::vector<Defender> &defenders, double last_round_p_loot);
        static int64_t ransom_cost(int64_t _assets); 
        static int64_t recovery_cost(int64_t _assets);

        static int64_t expected_loss(int64_t investment, int64_t assets_, int64_t capex_, double est_p_attack);
        static double gsl_expected_loss_wrapper(double x, void* params);
        static bool expected_loss_contains_minimum(int64_t investment, int64_t assets_, int64_t capex_, double est_p_attack);

        static double posture_if_investment(int64_t investment, int64_t assets_, int64_t capex_);
        static double d_posture_if_investment(int64_t investment, int64_t assets_, int64_t capex_);
        // double d_d_posture_if_investment(int64_t investment);

        static double probability_of_loss(int64_t investment, int64_t assets_, int64_t capex_, double est_p_attack);
        static double d_probability_of_loss(int64_t investment, int64_t assets_, int64_t capex_, double est_p_attack);
        // double d_d_probability_of_loss(int64_t investment);

        static int64_t cost_if_attacked(int64_t investment, int64_t assets_);
        static double d_cost_if_attacked(int64_t investment, int64_t assets_);
        // double d_d_cost_if_attacked(int64_t investment);

        static std::vector<Insurer>* insurers; 
        static std::vector<uint32_t>* alive_insurers_indices; 

        static void reset();

    // non-static public definitions
    public: 
        Defender(int id_in, Params &p);
        void gain(int64_t gain) override;
        void lose(int64_t loss) override;

        uint64_t id;
        int64_t capex = 0;
        double posture;

        bool attacked = false; // Has this defender been attacked yet this round?

        double defender_specific_estimated_p_attack;
       
        
        bool insured = false;
        int ins_idx = -1;
        PolicyType policy;

        void security_depreciation();
        void choose_security_strategy();
        void submit_claim(uint32_t loss);
    
        double gsl_find_minimum();

    // non-static private definitions
    private:   
        void purchase_insurance_policy(Insurer* i, PolicyType p);
        void make_security_investment(uint32_t x);
};