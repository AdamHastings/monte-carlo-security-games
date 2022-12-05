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

        Defender();

        static void reset_ctr();

    private:
        static int s_ctr;
};

class Attacker : public Player {
    public:
        int id;
        std::map<int, double> victims;

        Attacker(double INEQUALITY);

        static void reset_ctr();

    private:
        static int s_ctr;
};

class Insurer : public Player {
    public:
        Insurer(){};
};

class Government : public Player {
    public:
        Government(){};
};