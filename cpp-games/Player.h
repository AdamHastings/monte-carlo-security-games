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