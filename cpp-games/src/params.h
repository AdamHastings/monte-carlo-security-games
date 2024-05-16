#pragma once

#include <string>
#include <vector>
#include "Distributions.h"

struct Params {
    Distribution* NUM_ATTACKERS_distribution;
    Distribution* INEQUALITY_distribution;
    Distribution* EFFICIENCY_distribution;
    Distribution* RANSOM_BASE_distribution;
    Distribution* RANSOM_EXP_distribution;
    Distribution* RECOVERY_COST_BASE_distribution;
    Distribution* RECOVERY_COST_EXP_distribution;
    Distribution* WEALTH_distribution;
    Distribution* POSTURE_distribution;

    Distribution* LOSS_RATIO_distribution;
    Distribution* RETENTION_REGRESSION_FACTOR_distribution;
    Distribution* NUM_DEFENDERS_distribution;
    Distribution* NUM_INSURERS_distribution;
    Distribution* ATTACKS_PER_EPOCH_distribution;
    
    Distribution* EPSILON_distribution;
    Distribution* DELTA_distribution;

    unsigned int NUM_GAMES;

    bool verbose;
    bool assertions_on;
    std::string logname;
};

class params_loader {
    public:
        static Params load_cfg(std::string basename);
};