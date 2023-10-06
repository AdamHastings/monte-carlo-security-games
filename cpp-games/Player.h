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

        static PolicyType provide_a_quote(double assets, double posture);

    private:
        double median_attacker_assets; // need to init somehow
};

class Defender : public Player {
    public:
        uint id;
        double posture;
        double costToAttack;
        std::map<int, double> claimsReceived;
        bool insured;

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

