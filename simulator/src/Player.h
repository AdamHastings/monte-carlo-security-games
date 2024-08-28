#pragma once

#include <map>
#include <cassert>
#include "params.h"

class Insurer;
class Defender;
class Attacker;

struct PolicyType {
    int64_t premium = -1;
    int64_t retention = -1;
};

class Player {
    public:
        int64_t assets;
        static Params p; // Could this be static? Shared by all players?

        Player(Params &p_in);
        virtual void gain(int64_t gain);
        virtual void lose(int64_t loss);
        
        int64_t get_assets();
        bool is_alive(); 
};