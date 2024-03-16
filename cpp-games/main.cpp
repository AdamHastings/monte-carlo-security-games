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
    // TODO or should we timestamp each run?
    ifstream f(fpath.c_str());
    if (f.good()) {
        std::cout << "\nThis file already exists: " << fpath << "\nDo you want to replace it (Y)? Or append to it (A)? Y/A/n\n >> ";
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
    vector<Params> v = params_loader::load_cfg(basename);

    auto start = std::chrono::system_clock::now();
    std::time_t start_time = std::chrono::system_clock::to_time_t(start);

    std::cout << "started " << v[0].NUM_GAMES << " games at " << std::ctime(&start_time);
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