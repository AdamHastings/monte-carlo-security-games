#pragma once

#include <map>
#include <cassert>
#include "params.h"

struct PolicyType {
    double premium;
    double retention;
};


class Player {
    public:
        double assets;
        Params p;

        Player(Params &p_in);
        void gain(double gain);
        void lose(double loss);
        double get_assets();
};

class Insurer : public Player {
    public:
        uint id;
        Insurer(int id_in, Params &p);

        PolicyType provide_a_quote(double assets, double posture, double estimated_costToAttackPercentile);

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
        std::map<int, double> claimsReceived;
        bool insured;

        // This variable is "hidden" to the defender and only known to insurers
        // We store it here so that each insurer doesn't need to recompute. 
        double costToAttackPercentile;

        Defender(int id_in, Params &p);
        void choose_security_strategy(Insurer i);


    private:
        void purchase_insurance_policy(Insurer &i, PolicyType p);
        void make_security_investment(double x);
};

class Attacker : public Player {
    public:
        uint id;
        std::map<int, double> victims;

        Attacker(int id_in,  Params &p);
};

