#include <fstream>
#include "params.h"




std::vector<Params> params_loader::load_cfg(std::string basename) {
    std::string fpath = "configs/" + basename + ".json";
    
    std::ifstream file(fpath);

    Json::Reader reader;
    Json::Value jsonData;
    reader.parse(file, jsonData);

    std::vector<Params> ret;
    for (int i=0; i<jsonData["NUM_GAMES"].asInt(); i++) {
        Params p;
    

        double draw = 0;
        Distribution* ATTACKERS_distribution = Distribution::createDistribution(jsonData["ATTACKERS"]);
        while (draw <= 0 || draw > 1 ) {
            draw = ATTACKERS_distribution->draw();
        }
        p.ATTACKERS = draw;

        draw = 0;
        Distribution* INEQUALITY_distribution = Distribution::createDistribution(jsonData["INEQUALITY"]);
        while (draw <= 0 || draw > 1 ) {
            draw = INEQUALITY_distribution->draw();
        }
        p.INEQUALITY = draw;

        // p.ATTACKERS  = Distribution::createDistribution(jsonData["ATTACKERS"]); // TODO this isn't working like I think it should...need to double check it
        // p.INEQUALITY = Distribution::createDistribution(jsonData["INEQUALITY"]);
        
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

        ret.push_back(p);
    }
    return ret;
}