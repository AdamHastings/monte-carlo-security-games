#include <fstream>
#include <cassert>
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
    p.RANSOM_B0_distribution = Distribution::createDistribution(jsonData["RANSOM_B0"]);
    p.RANSOM_B1_distribution = Distribution::createDistribution(jsonData["RANSOM_B1"]);
    p.RECOVERY_COST_BASE_distribution = Distribution::createDistribution(jsonData["RECOVERY_COST_BASE"]);
    p.RECOVERY_COST_EXP_distribution = Distribution::createDistribution(jsonData["RECOVERY_COST_EXP"]);
    p.WEALTH_distribution     = Distribution::createDistribution(jsonData["WEALTH"]);
    p.POSTURE_distribution    = Distribution::createDistribution(jsonData["POSTURE"]);    
    p.POSTURE_NOISE_distribution    = Distribution::createDistribution(jsonData["POSTURE_NOISE"]);   

    p.DEPRECIATION_distribution  = Distribution::createDistribution(jsonData["DEPRECIATION"]); 
 

    p.LOSS_RATIO_distribution                  = Distribution::createDistribution(jsonData["LOSS_RATIO"]);
    p.RETENTION_REGRESSION_FACTOR_distribution = Distribution::createDistribution(jsonData["RETENTION_REGRESSION_FACTOR"]);
    p.NUM_DEFENDERS_distribution               = Distribution::createDistribution(jsonData["NUM_DEFENDERS"]);
    
    Json::Value num_insurers_json = jsonData["NUM_INSURERS"];
    assert(num_insurers_json["distribution"] == "fixed");
    p.NUM_INSURERS_distribution = Distribution::createDistribution(num_insurers_json);

    Json::Value num_quotes_json = jsonData["NUM_QUOTES"];
    if (num_quotes_json["distribution"] == "truncated_normal"  || num_quotes_json["distribution"] == "fixed") {
        assert(num_quotes_json["mean"] <  num_insurers_json["val"]);
        num_quotes_json["min"] = 0;
        num_quotes_json["max"] = num_insurers_json["val"];
    }

    p.NUM_QUOTES_distribution = Distribution::createDistribution(num_quotes_json);

    p.ATTACKS_PER_EPOCH_distribution           = Distribution::createDistribution(jsonData["ATTACKS_PER_EPOCH"]);
    p.CTA_SCALING_FACTOR_distribution          = Distribution::createDistribution(jsonData["CTA_SCALING_FACTOR"]);


    p.DELTA_distribution   = Distribution::createDistribution(jsonData["DELTA"]);

    p.NUM_GAMES = jsonData["NUM_GAMES"].asInt();    


    p.verbose       = jsonData["verbose"].asBool();
    p.assertions_on = jsonData["assertions_on"].asBool();
    p.logname       = "logs/" + basename + ".csv";

    return p;
}