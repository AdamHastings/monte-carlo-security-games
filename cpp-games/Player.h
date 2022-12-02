#pragma once

#include <map>

class Player {
    public:
        float assets;

        Player();
        void gain(float gain);
        void lose(float loss);
        float get_assets();
};

class Defender : public Player {
    public:
        int id;
        float probAttackSuccess;
        float costToAttack;
        std::map<int, float> claimsReceived;

        Defender();

    private:
        static int s_ctr;
};

class Attacker : public Player {
    public:
        int id;
        std::map<int, float> victims;

        Attacker();

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