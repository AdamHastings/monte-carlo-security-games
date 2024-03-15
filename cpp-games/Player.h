#pragma once

#include <map>
#include <cassert>
#include "params.h"
// #include "Insurer.h"

class Insurer;
class Defender;
class Attacker;

struct PolicyType {
    double premium;
    double retention;
};



class Player {
    public:
        double assets;
        Params p;
        bool alive = true;

        Player(Params &p_in);
        virtual void gain(double gain);
        virtual void lose(double loss);
        double get_assets();
};



class Attacker : public Player {
    public:
        uint id;
        
        static uint a_init;
        static double attacker_iter_sum; // how much the attackers have cumulatively gained or lost this round
        static std::vector<double> cumulative_assets; // running total of all attackers' assets
        static double current_sum_assets; // sum total of all class instances

        std::map<int, double> victims;


        Attacker(int id_in,  Params &p);
        void gain(double gain) override;
        void lose(double loss) override;
};



class Defender : public Player {
    public:
        static double estimated_probability_of_attack;

        static uint d_init;
        static double defender_iter_sum;
        static std::vector<double> cumulative_assets; // running total of all defenders' assets
        static double current_sum_assets; // sum total of all class instances


        uint id;
        double posture;
        double costToAttack;
        // std::map<int, double> claimsReceived;
        bool insured;

        std::vector<Insurer>* insurers;
        int ins_idx = -1;
        
        // This variable is "hidden" to the defender and only known to insurers
        // We store it here so that each insurer doesn't need to recompute. 
        double costToAttackPercentile;

        Defender(int id_in, Params &p, std::vector<Insurer> &insurers);
        void gain(double gain) override;
        void lose(double loss) override;

        void choose_security_strategy();
        void submit_claim(double loss);



    private:
        void purchase_insurance_policy(Insurer &i, PolicyType p);
        void make_security_investment(double x);
};