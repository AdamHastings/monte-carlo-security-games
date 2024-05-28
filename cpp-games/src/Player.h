#pragma once

#include <map>
#include <cassert>
#include "params.h"

class Insurer;
class Defender;
class Attacker;

struct PolicyType {
    uint32_t premium;
    uint32_t retention;
};

class Player {
    public:
        uint32_t assets;
        Params p;

        Player(Params &p_in);
        virtual void gain(uint32_t gain);
        virtual void lose(uint32_t loss);
        uint32_t get_assets();

        bool is_alive(); 
};