#pragma once 

#include "params.h"
#include "Player.h"
#include "Insurer.h"


class Defender : public Player {
    public:
        static double estimated_probability_of_attack;

        static double d_init;
        static double defender_iter_sum;
        static std::vector<double> cumulative_assets; // running total of all defenders' assets
        static double current_sum_assets; // sum total of all class instances

        uint id;
        double posture;
        double costToAttack;
       
        std::vector<Insurer>* insurers;
        bool insured = false;
        int ins_idx = -1;
        PolicyType policy;
        
        // This variable is "hidden" to the defender and only known to insurers
        // We store it here so that each insurer doesn't need to recompute. 
        double costToAttackPercentile;

        Defender(int id_in, Params &p, std::vector<Insurer> &insurers);
        void gain(double gain) override;
        void lose(double loss) override;

        void choose_security_strategy();
        void submit_claim(double loss);


    private:
        void purchase_insurance_policy(Insurer* i, PolicyType p);
        void make_security_investment(double x);
};

