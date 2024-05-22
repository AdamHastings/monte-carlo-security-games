#pragma once
#include <vector>

class utils {
    public:
        static double computeMean(const std::vector<double>& data);
        static double computeVariance(const std::vector<double>& data, double mean);
};