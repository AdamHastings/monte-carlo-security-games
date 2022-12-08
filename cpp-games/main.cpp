#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <ctime>  
#include <cstdlib>
#include <experimental/filesystem>
#include <jsoncpp/json/json.h>
#include "oneapi/tbb.h"
#include "Player.h"
#include "Game.h"


using namespace oneapi::tbb;
using namespace std;


void RunGame(Params p) {
    
    Insurer insurer = Insurer();
    Government government = Government();

    // TODO put a lot of this into constructor
    // And pass in Insurer and Government as pointers.

    std::vector<Defender> defenders;
    for (int i=0; i < p.B; i++) {
        Defender d = Defender(i);

        double investment = d.assets * p.MANDATE;
        double selfless_investment = investment * p.TAX;
        double selfish_investment  = investment - selfless_investment;

        double tax = selfless_investment;
        d.lose(tax);
        government.gain(tax);

        double insurance = selfless_investment * p.PREMIUM;
        d.lose(insurance);
        insurer.gain(insurance);

        double personal_security_investment = selfish_investment - insurance;
        d.costToAttack = d.assets * p.EFFORT;
        d.costToAttack += personal_security_investment * p.EFFORT;
        d.lose(personal_security_investment);

        // std::cout << "Making new defenders with d.assets=" << d.assets << std::endl;
        defenders.push_back(Defender(d));
    }

    std::vector<Attacker> attackers;
    for (int i=0; i < p.B * p.ATTACKERS; i++) {
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

void init_logs(std::string basename) {

    std::string fpath = "logs/" + basename + ".csv";

    if (std::experimental::filesystem::exists(fpath)) {
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
    header += "d_init,"
    header += "d_end,"
    header += "a_init,"
    header += "a_end,"
    header += "i_init,"
    header += "i_end,"
    header += "g_init,"
    header += "g_end,"
    header += "attacks_attempted,"
    header += "attacks_succeeded,"
    header += "amount_stolen,"
    header += "attacker_expenditures,"
    header += "government_expenditures,"
    header += "crossovers,"
    header += "insurer_tod,"
    header += "paid_claims,"
    header += "final_iter,"
    header += "outcome"
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
    
    MANDATE_range.reserve(jsonData["MANDATE_range"].size());
    ATTACKERS_range.reserve(jsonData["ATTACKERS"].size());
    INEQUALITY_range.reserve(jsonData["INEQUALITY"].size());
    PREMIUM_range.reserve(jsonData["PREMIUM"].size());
    EFFICIENCY_range.reserve(jsonData["EFFICIENCY"].size());
    EFFORT_range.reserve(jsonData["EFFORT"].size());
    PAYOFF_range.reserve(jsonData["PAYOFF"].size());
    CAUGHT_range.reserve(jsonData["CAUGHT"].size());
    CLAIMS_range.reserve(jsonData["CLAIMS"].size());
    TAX_range.reserve(jsonData["TAX"].size());

    std::transform(jsonData["MANDATE_range"].begin(), jsonData["MANDATE_range"].end(), std::back_inserter(MANDATE_range), [](const auto& e) { return e.asDouble(); });
    std::transform(jsonData["ATTACKERS_range"].begin(), jsonData["ATTACKERS_range"].end(), std::back_inserter(ATTACKERS_range), [](const auto& e) {return e.asDouble(); });
    std::transform(jsonData["INEQUALITY_range"].begin(), jsonData["INEQUALITY_range"].end(), std::back_inserter(INEQUALITY_range), [](const auto& e) {return e.asDouble(); });
    std::transform(jsonData["PREMIUM_range"].begin(), jsonData["PREMIUM_range"].end(), std::back_inserter(PREMIUM_range), [](const auto& e) {return e.asDouble(); });
    std::transform(jsonData["EFFICIENCY_range"].begin(), jsonData["EFFICIENCY_range"].end(), std::back_inserter(EFFICIENCY_range), [](const auto& e) {return e.asDouble(); });
    std::transform(jsonData["EFFORT_range"].begin(), jsonData["EFFORT_range"].end(), std::back_inserter(EFFORT_range), [](const auto& e) {return e.asDouble(); });
    std::transform(jsonData["PAYOFF_range"].begin(), jsonData["PAYOFF_range"].end(), std::back_inserter(PAYOFF_range), [](const auto& e) {return e.asDouble(); });
    std::transform(jsonData["CAUGHT_range"].begin(), jsonData["CAUGHT_range"].end(), std::back_inserter(CAUGHT_range), [](const auto& e) {return e.asDouble(); });
    std::transform(jsonData["CLAIMS_range"].begin(), jsonData["CLAIMS_range"].end(), std::back_inserter(CLAIMS_range), [](const auto& e) {return e.asDouble(); });
    std::transform(jsonData["TAX_range"].begin(), jsonData["TAX_range"].end(), std::back_inserter(TAX_range), [](const auto& e) {return e.asDouble(); });

    std::vector<Params> ret;

    for (auto a : MANDATE_range) {
    for (auto b : ATTACKERS_range) {
    for (auto c : INEQUALITY_range) {
    for (auto d : PREMIUM_range) {
    for (auto e : EFFICIENCY_range) {
    for (auto f : EFFORT_range) {
    for (auto g : PAYOFF_range) {
    for (auto h : CAUGHT_range) {
    for (auto i : CLAIMS_range) {
    for (auto j : TAX_range) {

        Params p;
        p.MANDATE    = a;
        p.ATTACKERS  = b;
        p.INEQUALITY = c;
        p.PREMIUM    = d;
        p.EFFICIENCY = e;
        p.EFFORT     = f;
        p.PAYOFF     = g;
        p.CAUGHT     = h;
        p.CLAIMS     = i;
        p.TAX        = j;

        p.B          = jsonData["B"].asInt();
        p.N          = jsonData["N"].asInt();
        p.E          = jsonData["E"].asInt();
        p.D          = jsonData["D"].asInt();

        p.verbose    = jsonData["verbose"].asBool();
        p.logname   = "logs/" + basename + ".csv";

        ret.push_back(p);
    }}}}}}}}}}

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
    basename = basename.substr(0, basename.find("."));
    init_logs(basename);
    vector<Params> v = load_cfg(basename);

    auto start = std::chrono::system_clock::now();
    std::time_t start_time = std::chrono::system_clock::to_time_t(start);
    std::cout << "started " << std::to_string(v.size()) << " games at " << std::ctime(&start_time);

    ParallelRunGames(v);

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
 
    std::cout << "finished computation at " << std::ctime(&end_time)
              << "elapsed time: " << elapsed_seconds.count() << "s"
              << std::endl;

    return 0;
}