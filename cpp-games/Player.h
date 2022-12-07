#pragma once

#include <map>

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
        int id;
        double probAttackSuccess;
        double costToAttack;
        std::map<int, double> claimsReceived;

        Defender(int id_in);

        static void reset_ctr();

};

class Attacker : public Player {
    public:
        int id;
        std::map<int, double> victims;

        Attacker(int id_in, double INEQUALITY);

        static void reset_ctr();
};

class Insurer : public Player {
    public:
        Insurer(){};
};

class Government : public Player {
    public:
        Government(){};
};