#include <fstream>
#include "params.h"




Params params_loader::load_cfg(std::string basename) {
    std::string fpath = "configs/" + basename + ".json";
    
    std::ifstream file(fpath);

    Json::Reader reader;
    Json::Value jsonData;
    reader.parse(file, jsonData);

    Params p;
    
    p.NUM_ATTACKERS_distribution = Distribution::createDistribution(jsonData["NUM_ATTACKERS"]);
    p.INEQUALITY_distribution = Distribution::createDistribution(jsonData["INEQUALITY"]);
    p.EFFICIENCY_distribution = Distribution::createDistribution(jsonData["EFFICIENCY"]);
    p.PAYOFF_distribution     = Distribution::createDistribution(jsonData["PAYOFF"]);
    p.WEALTH_distribution     = Distribution::createDistribution(jsonData["WEALTH"]);
    p.POSTURE_distribution    = Distribution::createDistribution(jsonData["POSTURE"]);    

    p.LOSS_RATIO_distribution                  = Distribution::createDistribution(jsonData["LOSS_RATIO"]);
    p.RETENTION_REGRESSION_FACTOR_distribution = Distribution::createDistribution(jsonData["RETENTION_REGRESSION_FACTOR"]);
    p.NUM_DEFENDERS_distribution            = Distribution::createDistribution(jsonData["NUM_DEFENDERS"]);
    p.NUM_INSURERS_distribution                = Distribution::createDistribution(jsonData["NUM_INSURERS"]);

    p.EPSILON_distribution = Distribution::createDistribution(jsonData["EPSILON"]);
    p.DELTA_distribution   = Distribution::createDistribution(jsonData["DELTA"]);

    p.NUM_GAMES = jsonData["NUM_GAMES"].asInt();    


    p.verbose       = jsonData["verbose"].asBool();
    p.assertions_on = jsonData["assertions_on"].asBool();
    p.logname       = "logs/" + basename + ".csv";

    return p;
}