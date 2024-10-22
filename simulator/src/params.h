#pragma once

#include <string>
#include <vector>
#include "Distributions.h"

struct Params {
    Distribution* NUM_ATTACKERS_distribution;
    Distribution* INEQUALITY_distribution;
    Distribution* RANSOM_B0_distribution;
    Distribution* RANSOM_B1_distribution;
    Distribution* RECOVERY_COST_BASE_distribution;
    Distribution* RECOVERY_COST_EXP_distribution;
    Distribution* WEALTH_distribution;
    Distribution* POSTURE_distribution;
    Distribution* POSTURE_NOISE_distribution;


    Distribution* NUM_QUOTES_distribution;
    Distribution* LOSS_RATIO_distribution;
    Distribution* RETENTION_REGRESSION_FACTOR_distribution;
    Distribution* NUM_DEFENDERS_distribution;
    Distribution* NUM_INSURERS_distribution;
    Distribution* ATTACKS_PER_EPOCH_distribution;
    Distribution* CTA_SCALING_FACTOR_distribution;

    Distribution* DEPRECIATION_distribution;
    Distribution* TARGET_SECURITY_SPENDING_distribution;
    Distribution* INVESTMENT_SCALING_FACTOR_distribution;
    Distribution* MAX_ITERATIONS_distribution;
    Distribution* MANDATORY_INVESTMENT_distribution;
    Distribution* GROWTH_RATE_distribution;
    
    bool mandatory_insurance;
    
    bool sweep = false; // is this a sweep experiment?
    std::string sweepvar = "";
    
    unsigned int NUM_GAMES;

    bool verbose;
    bool assertions_on;
    std::string logname;
};

class params_loader {
    public:
        static Params load_cfg(std::string config_filename, std::string logname);
};