#pragma once

#include <string>
#include <vector>
#include "Distributions.h"

struct Params {
    Distribution* ATTACKERS_distribution;
    Distribution* INEQUALITY_distribution;
    Distribution* EFFICIENCY_distribution;
    Distribution* PAYOFF_distribution;
    Distribution* WEALTH_distribution;
    Distribution* POSTURE_distribution;

    double LOSS_RATIO;
    double RETENTION_REGRESSION_FACTOR;

    uint NUM_BLUE_PLAYERS;
    uint NUM_INSURERS;
    uint NUM_GAMES;
    uint EPSILON;
    uint DELTA;

    bool verbose;
    bool assertions_on;
    std::string logname;
};

class params_loader {
    public:
        static Params load_cfg(std::string basename);
};