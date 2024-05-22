#pragma once 

#include "params.h"
#include "Player.h"
#include "Insurer.h"


class Defender : public Player {
    public:
        static double estimated_probability_of_attack;

        static unsigned long long d_init;
        static unsigned long long defender_iter_sum;
        static std::vector<unsigned long long> cumulative_assets; // running total of all defenders' assets
        static unsigned long long current_sum_assets; // sum total of all class instances
        static unsigned long long sum_recovery_costs;
        static unsigned long long policiesPurchased;
        static unsigned long long defensesPurchased;
        // static unsigned int doNothing; // TODO implement? what is this for? maybe delete

        uint id;
        double posture;
       
        std::vector<Insurer>* insurers;
        bool insured = false;
        int ins_idx = -1;
        PolicyType policy;

        Defender(int id_in, Params &p, std::vector<Insurer> &insurers);
        void gain(uint32_t gain) override;
        void lose(uint32_t loss) override;

        void choose_security_strategy();
        void submit_claim(uint32_t loss);

        static void reset();


    private:
        void purchase_insurance_policy(Insurer* i, PolicyType p);
        void make_security_investment(uint32_t x);
};

