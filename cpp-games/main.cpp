#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <ctime>  
#include <cstdlib>
#include "json/json.h"
#include "oneapi/tbb.h"
#include "Player.h"
#include "Game.h"


using namespace oneapi::tbb;
using namespace std;

std::vector<double> MANDATE_range;
std::vector<double> ATTACKERS_range;
std::vector<double> INEQUALITY_range;
std::vector<double> PREMIUM_range;
std::vector<double> EFFICIENCY_range;
std::vector<double> EFFORT_range;
std::vector<double> PAYOFF_range;
std::vector<double> CAUGHT_range;
std::vector<double> CLAIMS_range;
std::vector<double> TAX_range;

void RunGame(Params p) {
    
    Insurer insurer = Insurer();
    Government government = Government();

    std::vector<Defender> defenders;
    for (int i=0; i < p.B; i++) {
        Defender d = Defender(i);

        double investment = d.assets * p.MANDATE;
        double selfless_investment = investment * p.TAX;
        double selfish_investment  = investment - selfless_investment;

        double tax = selfless_investment;
        d.lose(tax);
        government.gain(tax);

        double insurance = selfish_investment * p.PREMIUM;
        d.lose(insurance);
        insurer.gain(insurance);

        double personal_security_investment = selfish_investment - insurance;
        d.costToAttack = d.assets * p.EFFORT;
        d.costToAttack += personal_security_investment * p.EFFORT;
        d.lose(personal_security_investment);

        defenders.push_back(Defender(d));

        if (p.assertions_on) {
            assert(tax >= 0);
            assert(personal_security_investment >= 0);
            assert(insurance >= 0);
        }
    }

    std::vector<Attacker> attackers;
    for (int i=0; i < (p.B * p.ATTACKERS); i++) {
        Attacker a = Attacker(i, p.INEQUALITY);
        attackers.push_back(a);
    }

    Game g = Game(p, defenders, attackers, insurer, government);
    g.run_iterations();

    // Write response to log file;
    ofstream log;
    log.open(p.logname, ios::out | ios::app);
    log << g.to_string();
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

void RunChunkOfGames(Params p) {

    for (auto i : CLAIMS_range) {
    for (auto j : TAX_range) {
        p.CLAIMS     = i;
        p.TAX        = j;

        RunGame(p);
    }}
}

void SerialParallelRunGames(vector<Params> a) {
    
    parallel_for( blocked_range<int>(0,a.size()),
        [&](blocked_range<int> r) 
    {
            for(int i=r.begin(); i < r.end(); ++i)
            {
                RunChunkOfGames(a[i]);
            }
    });
}

void init_logs(std::string basename) {

    std::string fpath = "logs/" + basename + ".csv";

    std::cout << "Creating log " << fpath << std::endl;

    // Check if log file already exists so that we don't accidentally write over it
    ifstream f(fpath.c_str());
    if (f.good()) {
        std::cout << "\nThis file already exists: " << fpath << "\nDo you want to replace it? Y/n\n >> ";
        std::string response;
        std::cin >> response;
        if (response != "y" && response != "Y") {
            std::cout << "\nOK, this program will not overwrite " << fpath << ".\nThis program will now exit.\n";
            std::exit(0);
        }
    }

    ofstream log;

    std::string header = "";

    header += "MANDATE,";
    header += "ATTACKERS,";
    header += "INEQUALITY,";
    header += "PREMIUM,";
    header += "EFFICIENCY,";
    header += "EFFORT,";
    header += "PAYOFF,";
    header += "CAUGHT,";
    header += "CLAIMS,";
    header += "TAX,";

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
    log.open (fpath, ios::out);
    log << header;
    log.close();
}

std::vector<Params> load_cfg(std::string basename) {
    std::string fpath = "configs/" + basename + ".json";
    
    ifstream file(fpath);

    Json::Reader reader;
    Json::Value jsonData;
    reader.parse(file, jsonData);
  
    std::vector<Params> ret;

    for (auto a : jsonData["MANDATE_range"]) {MANDATE_range.push_back(a.asDouble());}
    for (auto a : jsonData["ATTACKERS_range"]) {ATTACKERS_range.push_back(a.asDouble());}
    for (auto a : jsonData["INEQUALITY_range"]) {INEQUALITY_range.push_back(a.asDouble());}
    for (auto a : jsonData["PREMIUM_range"]) {PREMIUM_range.push_back(a.asDouble());}
    for (auto a : jsonData["EFFICIENCY_range"]) {EFFICIENCY_range.push_back(a.asDouble());}
    for (auto a : jsonData["EFFORT_range"]) {EFFORT_range.push_back(a.asDouble());}
    for (auto a : jsonData["PAYOFF_range"]) {PAYOFF_range.push_back(a.asDouble());}
    for (auto a : jsonData["CAUGHT_range"]) {CAUGHT_range.push_back(a.asDouble());}
    for (auto a : jsonData["CLAIMS_range"]) {CLAIMS_range.push_back(a.asDouble());}
    for (auto a : jsonData["TAX_range"]) {TAX_range.push_back(a.asDouble());}

    for (auto a : MANDATE_range) {
    for (auto b : ATTACKERS_range) {
    for (auto c : INEQUALITY_range) {
    for (auto d : PREMIUM_range) {
    for (auto e : EFFICIENCY_range) {
    for (auto f : EFFORT_range) {
    for (auto g : PAYOFF_range) {
    for (auto h : CAUGHT_range) {

        // Important: Some parameter values are filled in later
        // Otherwise, a completed ret vector would be over a TB big!

        Params p;
        p.MANDATE    = a;
        p.ATTACKERS  = b;
        p.INEQUALITY = c;
        p.PREMIUM    = d;
        p.EFFICIENCY = e;
        p.EFFORT     = f;
        p.PAYOFF     = g;
        p.CAUGHT     = h;

        p.B          = jsonData["B"].asInt();
        p.N          = jsonData["N"].asInt();
        p.E          = jsonData["E"].asInt();
        p.D          = jsonData["D"].asInt();

        p.verbose       = jsonData["verbose"].asBool();
        p.assertions_on = jsonData["assertions_on"].asBool();
        p.logname       = "logs/" + basename + ".csv";

        ret.push_back(p);
    }}}}}}}}
    // }}}}

    return ret;
}

int main(int argc, char** argv) {

    // Validate the inputs
    if (argc != 2) {
        std::cerr << "\nERROR: Incorrect number of args!";
        std::cerr << "\nExample of how to run config test_medium (located in configs/):";
        std::cerr << "\n     $ ./run_games test_medium.json\n\n";
        std::exit(1);
    }

    std::string basename(argv[1]);
    basename = basename.substr(0, basename.find_last_of("."));
    init_logs(basename);

    // vector of *incomplete* params, due to constraints on memory.
    // params need to be completed in ParallelRunGames
    vector<Params> v = load_cfg(basename);

    auto start = std::chrono::system_clock::now();
    std::time_t start_time = std::chrono::system_clock::to_time_t(start);

    int num_games = v.size() * CLAIMS_range.size() * TAX_range.size();
    std::cout << "started " << std::to_string(num_games) << " games at " << std::ctime(&start_time);

    // ParallelRunGames(v);
    // SerialRunGames(v);
    SerialParallelRunGames(v);

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
 
    std::cout << "finished computation at " << std::ctime(&end_time)
              << "elapsed time: " << elapsed_seconds.count() << "s"
              << std::endl;

    return 0;
}