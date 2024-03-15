#pragma once

#include <map>
#include <cassert>
#include "params.h"

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
        std::map<int, double> victims;

        double* attacker_iter_sum;

        Attacker(int id_in,  Params &p);
        void gain(double gain) override;
        void lose(double loss) override;
};

class Insurer : public Player {
    public:
        uint id;
        Insurer(int id_in, Params &p);
        void gain(double gain) override;
        void lose(double loss) override;

        PolicyType provide_a_quote(double assets, double posture, double estimated_costToAttackPercentile);
        double issue_payment(double claim);

        double* insurer_iter_sum;


        static std::vector<double> attacker_assets;
        static int num_attackers;
        static int num_defenders;
};

class Defender : public Player {
    public:
        static double estimated_probability_of_attack;

        uint id;
        double posture;
        double costToAttack;
        // std::map<int, double> claimsReceived;
        bool insured;

        double* defender_iter_sum;
        std::vector<Insurer>* insurers;
        // Insurer* insurer = NULL; 
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