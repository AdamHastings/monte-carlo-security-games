#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <ctime>  
#include <cstring>
#include <cstdlib>
#include <thread>
#include <random>
#include <unistd.h>
#include <sys/wait.h>
#include "json/json.h"
#include "Player.h"
#include "Game.h"
#include "Distributions.h"
using namespace std;


void RunGame(Params p, unsigned int game_number) {
    
    // Create and run the game
    Game g = Game(p, game_number);
    g.run_iterations();

    // Write response to log file;
    ofstream log;
    log.open(p.logname, ios::app);
    log << g.to_string();
    log.close();
}

void ParallelRunGames(Params p) {
    unsigned int num_cores = std::thread::hardware_concurrency(); // Get the number of cores on this system
    unsigned int maxProcesses = num_cores*4;

    unsigned int processesRunning = 0;

    for (uint i=0; i < p.NUM_GAMES; i++) {
        // Fork a child process for each job
        pid_t pid = fork();

        if (pid < 0) {
            // Fork failed
            std::cerr << "Fork failed!" << std::endl;
            exit(1);
        } else if (pid == 0) {
            // Child process
            RunGame(p, i);
            exit(0); // Terminate the child process
        } else {
            // Parent process
            processesRunning++;

            // Check if we've reached the maximum number of processes
            if (processesRunning >= maxProcesses) {
                // Wait for any child process to finish
                wait(NULL);
                processesRunning--;
            }
        }
    }

    // Wait for all remaining child processes to finish
    while (wait(NULL) > 0) {
        processesRunning--;
    }

    assert(processesRunning == 0);
}

void SerialRunGames(std::string basename) {
    string config_filename = "configs/" + basename + ".json";
    Params p = params_loader::load_cfg(config_filename);
        
    delete p.NUM_ATTACKERS_distribution;
    delete p.INEQUALITY_distribution;
    delete p.RANSOM_B0_distribution;
    delete p.RANSOM_B1_distribution;
    delete p.RECOVERY_COST_BASE_distribution;
    delete p.RECOVERY_COST_EXP_distribution;
    delete p.WEALTH_distribution;     
    delete p.POSTURE_distribution; 
    delete p.POSTURE_NOISE_distribution;
    delete p.NUM_QUOTES_distribution;
    delete p.LOSS_RATIO_distribution;
    delete p.RETENTION_REGRESSION_FACTOR_distribution;
    delete p.NUM_DEFENDERS_distribution;
    delete p.NUM_INSURERS_distribution;
    delete p.CTA_SCALING_FACTOR_distribution;
    delete p.ATTACKS_PER_EPOCH_distribution;
    delete p.DEPRECIATION_distribution;
    delete p.TARGET_SECURITY_SPENDING_distribution;
    delete p.INVESTMENT_SCALING_FACTOR_distribution;
    delete p.MAX_ITERATIONS_distribution;
    delete p.MANDATORY_INVESTMENT_distribution;
    delete p.GROWTH_RATE_distribution;

    for (uint i=0; i < p.NUM_GAMES; i++) {
        p = params_loader::load_cfg(config_filename);
        RunGame(p, i);
    }
}

void init_logs(std::string basename, Params p) {

    std::string fpath = "logs/" + basename + ".csv";
    std::cout << "Creating " << fpath << std::endl;

    std::string header = "";

    if (p.sweep) {
        header += p.sweepvar;
        header += ",";
    }

    header += "d_init";
    header += ",d_end";
    header += ",a_init";
    header += ",a_end";
    header += ",i_init";
    header += ",i_end";
    header += ",attacksAttempted";
    header += ",attacksSucceeded";
    header += ",attackerLoots";
    header += ",d_sum_recovery_costs";
    header += ",attackerExpenditures";
    header += ",policiesPurchased";
    header += ",defensesPurchased";
    header += ",do_nothing";
    header += ",d_sum_security_investments";
    header += ",NUM_QUOTES";
    header += ",MANDATORY_INVESTMENT";
    header += ",sum_premiums_collected";
    header += ",paid_claims";
    header += ",insurer_expenditures";
    header += ",final_iter";
    header += ",outcome";

    if (p.verbose) {
        header += ",d_cumulative_assets";
        header += ",a_cumulative_assets";
        header += ",i_cumulative_assets";
        header += ",num_alive_defenders";
        header += ",num_alive_attackers";
        header += ",num_alive_insurers";
        header += ",cumulative_round_policies_purchased";
        header += ",cumulative_round_defenses_purchased";
        header += ",cumulative_round_do_nothing";
        header += ",p_pairing";
        header += ",p_attacked";
        header += ",p_looted";
        // header += ",insurer_estimate_p_pairing";
        // header += ",estimated_probability_of_attack";
        header += ",cumulative_defender_avg_posture";
    }

    header += "\n";

    ofstream log;

    // Check if log file already exists so that we don't accidentally write over it
    ifstream f(fpath.c_str());
    if (f.good() && !basename.compare(0, 4, "test") == 0) {
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
        std::cerr << "\n     $ ./run/debug/run_games configs/test_medium.json\n\n";
        std::exit(1);
    }
    
    Params p = params_loader::load_cfg(argv[1]);
    
    std::string basename(argv[1]);
    basename.erase(0, strlen("configs/"));
    basename.erase(basename.find_last_of("."));
    init_logs(basename, p);


    auto start = std::chrono::system_clock::now();
    std::time_t start_time = std::chrono::system_clock::to_time_t(start);
    std::cout << "started " << p.NUM_GAMES << " games at " << std::ctime(&start_time);
    
    #ifdef RELEASE
        ParallelRunGames(p); // run fast 
    #else
        SerialRunGames(basename); // run for easy debug
    #endif

    delete p.NUM_ATTACKERS_distribution;
    delete p.INEQUALITY_distribution;
    delete p.RANSOM_B0_distribution;
    delete p.RANSOM_B1_distribution;
    delete p.RECOVERY_COST_BASE_distribution;
    delete p.RECOVERY_COST_EXP_distribution;
    delete p.WEALTH_distribution;     
    delete p.POSTURE_distribution; 
    delete p.POSTURE_NOISE_distribution;
    delete p.NUM_QUOTES_distribution;
    delete p.LOSS_RATIO_distribution;
    delete p.RETENTION_REGRESSION_FACTOR_distribution;
    delete p.NUM_DEFENDERS_distribution;
    delete p.NUM_INSURERS_distribution;
    delete p.ATTACKS_PER_EPOCH_distribution;
    delete p.CTA_SCALING_FACTOR_distribution;
    delete p.DEPRECIATION_distribution;
    delete p.TARGET_SECURITY_SPENDING_distribution;
    delete p.INVESTMENT_SCALING_FACTOR_distribution;
    delete p.MAX_ITERATIONS_distribution;
    delete p.MANDATORY_INVESTMENT_distribution;
    delete p.GROWTH_RATE_distribution;

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
 
    std::cout << "finished computation at " << std::ctime(&end_time)
              << "elapsed time: " << elapsed_seconds.count() << "s"
              << std::endl;

    return 0;
}