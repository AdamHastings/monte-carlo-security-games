#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <ctime>  
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <random>
#include "Distributions.h"

using namespace std;

std::mt19937 Distribution::generator; // Standard mersenne_twister_engine seeded with rd()

void Distribution::seed(unsigned int seed){
    generator.seed(seed);
}

UniformRealDistribution::UniformRealDistribution(double _min, double _max) : dist(_min, _max) {}

double UniformRealDistribution::draw() {
    return dist(generator);
}

NormalDistribution::NormalDistribution(double _mean, double _stddev) : dist(_mean, _stddev) {}

double NormalDistribution::draw() {
    return dist(generator);
}

double NormalDistribution::mean() {
    return dist.mean();
}

LogNormalDistribution::LogNormalDistribution(double _mean, double _stddev) : dist(_mean, _stddev) {}

double LogNormalDistribution::draw() {
    return dist(generator);
}

TruncatedNormalDistribution::TruncatedNormalDistribution(double _mean, double _stddev, double _min, double _max) : dist(_mean, _stddev) {
    min = _min;
    max = _max;
 }

double TruncatedNormalDistribution::draw() {
    double draw;
    while (true) {
        draw = dist(generator);
        if (draw >= min && draw <= max) {
            break;
        }
        // } else {
        //     std::cout << " -- truncated normal draw out of range! Re-drawing..." << std::endl;
        // } // Could be a source of hanging programs...uncomment to find out
    }
    return draw;
 }

 double TruncatedNormalDistribution::mean() {
    return dist.mean();
 }

PoissonDistribution::PoissonDistribution(double _lambda) : dist(_lambda) {}

double PoissonDistribution::draw() {
    return dist(generator);
}

double PoissonDistribution::mean() {
    return dist.mean();
}

FixedDistribution::FixedDistribution(double _val) {
    val = _val;
}

double FixedDistribution::draw() {
    return val;
}

double FixedDistribution::mean() {
    return val;
}

// SweepDistribution::SweepDistribution(double _min, double _max, double _step) {
//     min = _min;
//     max = _max;
//     step = _step;
//     next_val = min;
// }

// double SweepDistribution::draw() {
//     double ret = next_val;
//     assert(ret <= max);
//     assert(ret >= min);

//     next_val += step;

//     return ret;
// }

// double SweepDistribution::mean() {
//     return (min + max) / 2;
// }


Distribution* Distribution::createDistribution(Json::Value d) {
    Distribution* dist;

    if (d["distribution"] == "uniform") {
        if (!d["min"] || !d["max"]) {
            std::cerr << "Must provide \"min\" and \"max\" with uniform distribution in config file" << std::endl;
            std::cerr << "Offending parameter: " << d << endl;
            exit(2);
        }
        double min = d["min"].asDouble();
        double max = d["max"].asDouble();
        dist = new UniformRealDistribution(min, max);
    } else if (d["distribution"] == "normal") {
        if (!d["mean"] || !d["stddev"]) {
            std::cerr << "Must provide \"mean\" and \"stddev\" with normal distribution in config file" << std::endl;
            std::cerr << "Offending parameter: " << d << endl;
            exit(2);
        }
        double mean = d["mean"].asDouble();
        double stddev = d["stddev"].asDouble();
        dist = new NormalDistribution(mean, stddev);
    } else if (d["distribution"] == "lognormal") {
        if (!d["mean"] || !d["stddev"]) {
            std::cerr << "Must provide \"mean\" and \"stddev\" with lognormal distribution in config file" << std::endl;
            std::cerr << "Offending parameter: " << d << endl;
            exit(2);
        }
        double mean = d["mean"].asDouble();
        double stddev = d["stddev"].asDouble();
        dist = new LogNormalDistribution(mean, stddev);
    } else if (d["distribution"] == "truncated_normal") {
        if (!d["mean"] || !d["stddev"] || !d["min"] || !d["max"]) {
            std::cerr << "Must provide \"mean\", \"stddev\", \"min\", and \"max\" with truncated_normal distribution in config file" << std::endl;
            std::cerr << "Offending parameter: " << d << endl;
            exit(2);
        }
        double mean   = d["mean"].asDouble();
        double stddev = d["stddev"].asDouble();
        double min    = d["min"].asDouble();
        double max    = d["max"].asDouble();
        assert(mean >= min);
        assert(mean <= max);
        dist = new TruncatedNormalDistribution(mean, stddev, min, max);
    } else if (d["distribution"] == "poisson") {
        if (!d["lambda"]) {
            std::cerr << "Must provide \"lambda\" with poisson distribution in config file" << std::endl;
            std::cerr << "Offending parameter: " << d << endl;
            exit(2);
        }
        double lambda   = d["lambda"].asDouble();
        dist = new PoissonDistribution(lambda);
    } else if (d["distribution"] == "fixed") {
        double val = d["val"].asDouble();
        dist = new FixedDistribution(val);
    } else {
        std::cerr << "unknown distribution type specified. Terminating..." << std::endl;
        std::cerr << "Offending parameter: " << d << endl;
        exit(1);
    }
    return dist;
}

double Distribution::mean() {
    // Some child classes do not have means so this cannot be a pure virtual function
    // But only the child function should ever be called, if it has a mean function
    // So we crash here if the parent function is ever called
    assert(false);
    return 0;
}

Distribution::~Distribution(){}