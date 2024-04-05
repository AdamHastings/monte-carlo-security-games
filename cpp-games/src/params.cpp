#include <fstream>
#include "params.h"




Params params_loader::load_cfg(std::string basename) {
    std::string fpath = "configs/" + basename + ".json";
    
    std::ifstream file(fpath);

    Json::Reader reader;
    Json::Value jsonData;
    reader.parse(file, jsonData);

    Params p;
    
    p.ATTACKERS_distribution = Distribution::createDistribution(jsonData["ATTACKERS"]);
    p.INEQUALITY_distribution = Distribution::createDistribution(jsonData["INEQUALITY"]);
    p.EFFICIENCY_distribution = Distribution::createDistribution(jsonData["EFFICIENCY"]);
    p.PAYOFF_distribution     = Distribution::createDistribution(jsonData["PAYOFF"]);
    p.WEALTH_distribution     = Distribution::createDistribution(jsonData["WEALTH"]);
    p.POSTURE_distribution    = Distribution::createDistribution(jsonData["POSTURE"]);        

    p.NUM_BLUE_PLAYERS        = jsonData["NUM_BLUE_PLAYERS"].asInt();
    p.NUM_INSURERS            = jsonData["NUM_INSURERS"].asInt();
    p.NUM_GAMES               = jsonData["NUM_GAMES"].asInt();
    p.EPSILON                 = jsonData["EPSILON"].asInt();
    p.DELTA                   = jsonData["DELTA"].asInt();  

    p.verbose       = jsonData["verbose"].asBool();
    p.assertions_on = jsonData["assertions_on"].asBool();
    p.logname       = "logs/" + basename + ".csv";

    return p;
}