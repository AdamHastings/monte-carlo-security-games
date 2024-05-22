#include <algorithm>
#include <numeric>
#include <cmath>
#include <vector>
#include "utils.h"

// Function to compute the mean of a vector
double utils::computeMean(const std::vector<double>& data) {
    double sum = 0.0;
    for (const auto& value : data) {
        sum += value;
    }
    return sum / data.size();
}

// Function to compute the variance of a vector
double utils::computeVariance(const std::vector<double>& data, double mean) {
    double sumSquaredDiff = 0.0;
    for (const auto& value : data) {
        sumSquaredDiff += pow((value - mean), 2);
    }
    return sumSquaredDiff / (data.size() - 1);
}