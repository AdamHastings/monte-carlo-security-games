#pragma once
#include <vector>

class utils {
    public:
        static double computeMean(const std::vector<double>& data);
        static double computeVariance(const std::vector<double>& data, double mean);
        static double compute_mu_mom_lognormal(const std::vector<double>& data);
        static double compute_var_mom_lognormal(const std::vector<double>& data);
};