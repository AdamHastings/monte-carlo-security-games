#pragma once

#include <string>
#include "Distributions.h"

struct Params {
    double ATTACKERS;   // Only need to sample once per game
    double INEQUALITY;  // Only need to sample once per game 
    Distribution* EFFICIENCY_distribution;
    Distribution* PAYOFF_distribution;
    Distribution* WEALTH_distribution;
    Distribution* POSTURE_distribution;

    int NUM_BLUE_PLAYERS;
    int NUM_INSURERS;
    int NUM_GAMES;
    int EPSILON;
    int DELTA;

    bool verbose;
    bool assertions_on;
    std::string logname;
};