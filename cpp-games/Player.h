#pragma once

#include <map>
#include <cassert>


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
        double probAttackSuccess;
        double costToAttack;
        std::map<int, double> claimsReceived;

        Defender(int id_in);
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
};

class Government : public Player {
    public:
        Government(){};
};