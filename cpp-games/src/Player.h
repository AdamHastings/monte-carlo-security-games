#pragma once

#include <map>
#include <cassert>
#include "params.h"

class Insurer;
class Defender;
class Attacker;

struct PolicyType {
    int64_t premium;
    int64_t retention;
};

class Player {
    public:
        int64_t assets;
        Params p;

        Player(Params &p_in);
        virtual void gain(int64_t gain);
        virtual void lose(int64_t loss);
        int64_t get_assets();

        bool is_alive(); 
};