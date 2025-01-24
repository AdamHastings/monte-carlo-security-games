#include <algorithm>
#include <numeric>
#include <cmath>
#include <cassert>
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

// Method of moments to compute the mean of a lognormal distribution
// source: pp. 11--12 https://web.archive.org/web/20180423000433id_/https://scholarsarchive.byu.edu/cgi/viewcontent.cgi?article=2927&context=etd
double utils::compute_mu_mom_lognormal(const std::vector<double>& data) {
    double sum = 0;
    double sq_sum = 0;
    for (const auto& value : data) {
        sum += value;
        sq_sum += value * value;
    }
    double t1 = -0.5 * log(sq_sum);
    double t2 = 2 * log(sum);
    double t3 = -1.5 * log(data.size());
    
    return t1 + t2 + t3;
}

// Method of moments to compute the variance of a lognormal distribution
// source: pp. 11--12 https://web.archive.org/web/20180423000433id_/https://scholarsarchive.byu.edu/cgi/viewcontent.cgi?article=2927&context=etd
double utils::compute_var_mom_lognormal(const std::vector<double>& data) {
    double sum = 0;
    double sq_sum = 0;
    for (const auto& value : data) {
        sum += value;
        sq_sum += value * value;
    }
    double t1 = log(sq_sum);
    double t2 = -2 * log(sum);
    double t3 = log(data.size());

    return t1 + t2 + t3; 
}