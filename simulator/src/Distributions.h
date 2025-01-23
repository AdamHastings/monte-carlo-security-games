#pragma once

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
#include "json/json.h"

class Distribution {
    public:
        virtual double draw() =0; // pure virtual function must be implemented
        static Distribution* createDistribution(Json::Value d);
        virtual double mean();
        virtual ~Distribution();

        static void seed(unsigned int seed);

        virtual void step(); // virtual function may optionally be implemented

        static std::mt19937 generator; // Standard mersenne_twister_engine
};

class UniformRealDistribution : public Distribution {
    public: 
        std::uniform_real_distribution<double> dist;
        UniformRealDistribution(double _min, double _max);
        double draw() override;
};

class NormalDistribution : public Distribution {
    public: 
        std::normal_distribution<double> dist;
        NormalDistribution(double _mean, double _stddev);
        double draw();
        double mean() override;
};

class LogNormalDistribution : public Distribution {
    public:
        std::lognormal_distribution<double> dist;
        LogNormalDistribution(double _mean, double _stddev);
        double draw() override;
};

class TruncatedNormalDistribution : public Distribution {
    public: 
        std::normal_distribution<double> dist;
        double min;
        double max;
        TruncatedNormalDistribution(double _mean, double _stddev, double _min, double _max);
        double draw();
        double mean() override;
};

class PoissonDistribution : public Distribution {
    public: 
        std::poisson_distribution<int> dist;
        PoissonDistribution(double _lambda);
        double draw();
        double mean() override;
};

class FixedDistribution : public Distribution {
    public:
        double val;
        FixedDistribution(double _val);
        double draw();
        double mean() override;
};

class SweepDistribution : public Distribution {
    public:
        double next_val;
        double min;
        double max;
        double step;
        SweepDistribution(double _min, double _max, double _step);
        double draw();
        double mean() override;
};

class Sinusoid : public Distribution {
    public:
        double amplitude;
        double phase;
        double vertical_offset;
        int period; 
        Sinusoid(double _amplitude, double _period, double _phase, double _vertical_offset);
        double draw();
        double mean() override;
        void step() override;
    
    private:
        int iter;
};

class Sawtooth : public Distribution {
    public:
        double amplitude;
        double phase;
        double vertical_offset;
        int period; 
        Sawtooth(double _amplitude, double _period, double _phase, double _vertical_offset);
        double draw();
        double mean() override;
        void step() override;

    private:
        int iter;
};