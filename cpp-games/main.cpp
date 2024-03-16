#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <ctime>  
#include <cstring>
#include <cstdlib>
#include <random>
#include "json/json.h"
#include "oneapi/tbb.h"
#include "Player.h"
#include "Game.h"
#include "Distributions.h"

using namespace oneapi::tbb;
using namespace std;



void RunGame(Params p) {
    
    // std::cout << "constructing game" << std::endl;
    Game g = Game(p);
    // std::cout << "game constructed" << std::endl;    

    g.run_iterations();

    // Write response to log file;
    // TODO maybe handle all logging in its own file(s)
    ofstream log;
    log.open(p.logname, ios::app);
    log << g.to_string(); // TODO might need to check since the distribution format changed.
    log.close();
}

void ParallelRunGames(vector<Params> a) {
    parallel_for( blocked_range<int>(0,a.size()),
        [&](blocked_range<int> r) 
    {
            for(int i=r.begin(); i < r.end(); ++i)
            {
                RunGame(a[i]);
            }
    });
}

void SerialRunGames(vector<Params> a) {
    for (uint i=0; i < a.size(); i++) {
        RunGame(a[i]);
    }
}

void init_logs(std::string basename) {

    std::string fpath = "logs/" + basename + ".csv";

    std::cout << "Creating " << fpath << std::endl;

    ofstream log;

    std::string header = "";

    header += "ATTACKERS,";
    header += "INEQUALITY,";
    header += "EFFICIENCY,";
    header += "PAYOFF,";

    // TODO double check that this is correct
    header += "d_init,";
    header += "d_end,";
    header += "a_init,";
    header += "a_end,";
    header += "i_init,";
    header += "i_end,";
    header += "g_init,";
    header += "g_end,";
    header += "attacks_attempted,";
    header += "attacks_succeeded,";
    header += "amount_stolen,";
    header += "attacker_expenditures,";
    header += "government_expenditures,";
    header += "crossovers,";
    header += "insurer_tod,";
    header += "paid_claims,";
    header += "final_iter,";
    header += "outcome";
    header += "\n";

    // Check if log file already exists so that we don't accidentally write over it
    ifstream f(fpath.c_str());
    if (f.good()) {
        std::cout << "\nThis file already exists: " << fpath << "\nDo you want to replace it? Or append to it? Y/A/n\n >> ";
        std::string response;
        std::cin >> response;
        if (response == "a" || response == "A") {
            log.open (fpath, ios::app);
            log.close();
        } else if (response != "y" && response != "Y") {
            std::cout << "\nOK, this program will not overwrite " << fpath << ".\nThis program will now exit.\n";
            std::exit(0);
        } else {
            log.open (fpath, ios::out);
            log << header;
            log.close();
        }
    }  else {
        log.open (fpath, ios::out);
        log << header;
        log.close();
    }  
}


std::vector<Params> load_cfg(std::string basename) {
    std::string fpath = "configs/" + basename + ".json";
    
    ifstream file(fpath);

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


int main(int argc, char** argv) {

    // Validate the inputs
    if (argc != 2) {
        std::cerr << "\nERROR: Incorrect number of args!";
        std::cerr << "\nExample of how to run config test_medium (located in configs/):";
        std::cerr << "\n     $ ./run_games configs/test_medium.json\n\n";
        std::exit(1);
    }

    std::string basename(argv[1]);
    basename.erase(0, strlen("configs/"));
    basename.erase(basename.find_last_of("."));
    init_logs(basename);

    // vector of *incomplete* params, due to constraints on memory. IF UNIFORM PARAMETERS.
    // params need to be completed in ParallelRunGames
    vector<Params> v = load_cfg(basename);

    auto start = std::chrono::system_clock::now();
    std::time_t start_time = std::chrono::system_clock::to_time_t(start);

    std::cout << "started " << v[0].NUM_GAMES << " nonuniform games at " << std::ctime(&start_time);
    // ParallelRunGames(v); // TODO undo after debugging
    SerialRunGames(v);

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
 
    std::cout << "finished computation at " << std::ctime(&end_time)
              << "elapsed time: " << elapsed_seconds.count() << "s"
              << std::endl;

    return 0;
}