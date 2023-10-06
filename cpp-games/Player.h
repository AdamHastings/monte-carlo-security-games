#pragma once

#include <map>
#include <cassert>

struct PolicyType {
    double premium;
    double retention;
};


class Player {
    public:
        double assets;

        Player();
        void gain(double gain);
        void lose(double loss);
        double get_assets();
};

class Defender : public Player {
    public:
        uint id;
        double posture;
        double costToAttack;
        std::map<int, double> claimsReceived;
        bool insured;

        Defender(int id_in);

    private:
        void choose_security_strategy();
};

class Attacker : public Player {
    public:
        uint id;
        std::map<int, double> victims;

        Attacker(int id_in, double INEQUALITY);
};

class Insurer : public Player {
    public:
        Insurer(){};

        static PolicyType provide_a_quote(double assets, double posture);

    private:
        double median_attacker_assets; // need to init somehow
};

class Government : public Player {
    public:
        Government(){};
};