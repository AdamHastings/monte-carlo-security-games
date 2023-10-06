#pragma once

#include <string>
#include "Distributions.h"

struct Params {
    double ATTACKERS;
    double INEQUALITY;
    Distribution* EFFICIENCY_distribution;
    Distribution* PAYOFF_distribution;
    Distribution* WEALTH_distribution;
    Distribution* POSTURE_distribution;

    int B;
    int NUM_GAMES;
    int E;
    int D;

    bool verbose;
    bool assertions_on;
    std::string logname;
};