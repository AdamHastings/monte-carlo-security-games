#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <ctime>  
#include <cstdlib>
#include <experimental/filesystem>
#include "oneapi/tbb.h"
#include "Player.h"
#include "Game.h"


using namespace oneapi::tbb;
using namespace std;


class cfg {
    public:
        // vector<double> MANDATE_range    = {0.1, 0.2, 0.3, 0.4, 0.5};
        // vector<double> ATTACKERS_range  = {0.1, 0.2};
        // vector<double> INEQUALITY_range = {0.1};
        // vector<double> PREMIUM_range    = {0.1, 0.9};
        // vector<double> EFFICIENCY_range = {0.1};
        // vector<double> EFFORT_range     = {0.1};
        // vector<double> PAYOFF_range     = {0.1};
        // vector<double> CAUGHT_range     = {0.1};
        // vector<double> CLAIMS_range     = {0.1};
        // vector<double> TAX_range        = {0.1};

        // // test_medium
        vector<double> MANDATE_range    = {0.1, 0.5, 0.9};
        vector<double> ATTACKERS_range  = {0.1, 0.5, 1.0};
        vector<double> INEQUALITY_range = {0.1, 0.5, 1.0};
        vector<double> PREMIUM_range    = {0.0, 0.5, 1.0};
        vector<double> EFFICIENCY_range = {0.1, 0.5, 1.0};
        vector<double> EFFORT_range     = {0.1, 0.5, 1.0};
        vector<double> PAYOFF_range     = {0.1, 0.5, 1.0};
        vector<double> CAUGHT_range     = {0.1, 0.5, 1.0};
        vector<double> CLAIMS_range     = {0.1, 0.5, 1.0};
        vector<double> TAX_range        = {0.1, 0.5, 1.0};

        // test_small
        // vector<double> MANDATE_range    = {0.1, 0.8};
        // vector<double> ATTACKERS_range  = {0.5, 1.0};
        // vector<double> INEQUALITY_range = {0.3, 1.0};
        // vector<double> PREMIUM_range    = {0.0, 0.5, 1.0};
        // vector<double> EFFICIENCY_range = {0.1, 1.0};
        // vector<double> EFFORT_range     = {0.1, 0.4};
        // vector<double> PAYOFF_range     = {0.3, 0.9};
        // vector<double> CAUGHT_range     = {0.1, 0.6};
        // vector<double> CLAIMS_range     = {0.1, 1.0};
        // vector<double> TAX_range        = {0.0, 0.5, 1.0};

        // test_tiny
        // vector<double> MANDATE_range    = {0.1, 0.8};
        // vector<double> ATTACKERS_range  = {0.5}; // TODO change to 0.5 to realign with test_tiny
        // vector<double> INEQUALITY_range = {0.3};
        // vector<double> PREMIUM_range    = {0.0, 1.0};
        // vector<double> EFFICIENCY_range = {0.1};
        // vector<double> EFFORT_range     = {0.1, 0.4};
        // vector<double> PAYOFF_range     = {0.3, 0.9};
        // vector<double> CAUGHT_range     = {0.1, 0.6};
        // vector<double> CLAIMS_range     = {0.1, 1.0};
        // vector<double> TAX_range        = {0.0, 1.0};

        // test_target
        // vector<double> MANDATE_range    = {0.5};
        // vector<double> ATTACKERS_range  = {0.1}; // TODO change to 0.5 to realign with test_tiny
        // vector<double> INEQUALITY_range = {0.1};
        // vector<double> PREMIUM_range    = {0.5};
        // vector<double> EFFICIENCY_range = {0.5};
        // vector<double> EFFORT_range     = {1.0};
        // vector<double> PAYOFF_range     = {1.0};
        // vector<double> CAUGHT_range     = {0.5};
        // vector<double> CLAIMS_range     = {1.0};
        // vector<double> TAX_range        = {1.0};


        int B = 1000;
        int N = 1000;
        int E = 100; // error if E > D...why??? see lines 120--121...
        int D = 50;

        bool verbose = false;
        std::string filename = "logs/test.csv";


        // cfg::cfg/() {} // Nothing to initialize for now

        vector<Params> make_all_params() {
            
            vector<Params> ret;

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

                p.B          = B;
                p.N          = N;
                p.E          = E;
                p.D          = D;

                p.verbose    = verbose;
                p.filename   = filename;

                ret.push_back(p);
            }}}}}}}}}}

            return ret;
        }
};

// TODO make params its own class
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

    // std::string filename = "logs/test.csv";
    std::string filename = p.filename;
    ofstream log;
    log.open (filename, ios::out | ios::app);
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
    for (int i=0; i<a.size(); i++) {
        RunGame(a[i]);
    }
}


void init_logs(cfg &c) {
    std::string fpath =  c.filename;
    // std::string fpath = "logs/test.csv";
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
    header += "d_init,d_end,a_init,a_end,i_init,i_end,g_init,g_end,attacks_attempted,attacks_succeeded,amount_stolen,attacker_expenditures,government_expenditures,crossovers,insurer_tod,paid_claims,final_iter,outcome";
    header += "\n";
    log.open (fpath, ios::out);
    log << header;
    log.close();
}

int main() {

    cfg c = cfg();
    vector<Params> cart = c.make_all_params();

    init_logs(c);

    auto start = std::chrono::system_clock::now();
    std::time_t start_time = std::chrono::system_clock::to_time_t(start);
    std::cout << "started " << std::to_string(cart.size()) << " games at " << std::ctime(&start_time);

    ParallelRunGames(cart);
    // SerialRunGames(cart);

    auto end = std::chrono::system_clock::now();
 
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
 
    std::cout << "finished computation at " << std::ctime(&end_time)
              << "elapsed time: " << elapsed_seconds.count() << "s"
              << std::endl;

    return 0;
}