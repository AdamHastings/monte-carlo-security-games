#pragma once 

#include "params.h"
#include "Player.h"
#include "Insurer.h"


class Defender : public Player {
    public:
        static double estimated_probability_of_attack;

        static int64_t d_init;
        static int64_t defender_iter_sum;
        static std::vector<unsigned long long> cumulative_assets; // running total of all defenders' assets
        static int64_t current_sum_assets; // sum total of all class instances
        static int64_t sum_recovery_costs;
        static int64_t policiesPurchased;
        static int64_t defensesPurchased;
        static int64_t sum_security_investments;
        // static unsigned int doNothing; // TODO implement? what is this for? maybe delete
        
        static std::mt19937* gen;

        static double ransom_b0;
        static double ransom_b1;
        static double recovery_base;
        static double recovery_exp;

        static uint32_t NUM_QUOTES;

        static void perform_market_analysis(int prevRoundAttacks, int num_current_defenders);
        static long long ransom(int assets); // TODO add bounds check assertion
        static long long recovery_cost(int assets); // TODO add bounds check assertion

        static void reset();

    public: 
        Defender(int id_in, Params &p, std::vector<Insurer> &insurers);
        void gain(int64_t gain) override;
        void lose(int64_t loss) override;

        uint64_t id;
        double posture;
       
        std::vector<Insurer>* insurers;
        bool insured = false;
        int ins_idx = -1;
        PolicyType policy;

        void choose_security_strategy();
        void submit_claim(uint32_t loss);


    private:   
        void purchase_insurance_policy(Insurer* i, PolicyType p);
        void make_security_investment(uint32_t x);
        double find_optimal_investment();
        double posture_if_investment(double investment);
        void security_depreciation();
};

