#pragma once

#include "json/json.h"

class Distribution {
    public:
        virtual double draw() =0;
        
        static Distribution* createDistribution(Json::Value d);
        virtual double mean();
};