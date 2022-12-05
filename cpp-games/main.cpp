#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <ctime>  
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

        // vector<double> MANDATE_range    = {0.1, 0.5, 0.9};
        // vector<double> ATTACKERS_range  = {0.1, 0.5, 1.0};
        // vector<double> INEQUALITY_range = {0.1, 0.5, 1.0};
        // vector<double> PREMIUM_range    = {0.0, 0.5, 1.0};
        // vector<double> EFFICIENCY_range = {0.1, 0.5, 1.0};
        // vector<double> EFFORT_range     = {0.1, 0.5, 1.0};
        // vector<double> PAYOFF_range     = {0.1, 0.5, 1.0};
        // vector<double> CAUGHT_range     = {0.1, 0.5, 1.0};
        // vector<double> CLAIMS_range     = {0.1, 0.5, 1.0};
        // vector<double> TAX_range        = {0.1, 0.5, 1.0};

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

        // test_tiny2
        vector<double> MANDATE_range    = {0.1, 0.8};
        vector<double> ATTACKERS_range  = {0.1}; // TODO change to 0.5 to realign with test_tiny
        vector<double> INEQUALITY_range = {0.3};
        vector<double> PREMIUM_range    = {0.0, 1.0};
        vector<double> EFFICIENCY_range = {0.1};
        vector<double> EFFORT_range     = {0.1, 0.4};
        vector<double> PAYOFF_range     = {0.3, 0.9};
        vector<double> CAUGHT_range     = {0.1, 0.6};
        vector<double> CLAIMS_range     = {0.1, 1.0};
        vector<double> TAX_range        = {0.0, 1.0};

        int B = 100;
        int N = 10;
        int E = 100;
        int D = 50;

        bool verbose = false;


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
                p.E          = D;
                p.D          = E;

                p.verbose    = verbose;

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

    Defender::reset_ctr();
    std::vector<Defender> defenders;
    for (int i=0; i < p.B; i++) {
        Defender d = Defender();

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

        std::cout << "Making new defenders with d.assets=" << d.assets << std::endl;
        defenders.push_back(Defender(d));
    }

    Attacker::reset_ctr();
    std::vector<Attacker> attackers;
    for (int i=0; i < p.B * p.ATTACKERS; i++) {
        Attacker a = Attacker(p.INEQUALITY);
        attackers.push_back(a);
    }

    Game g = Game(p, defenders, attackers, insurer, government);
    g.run_iterations();

    // Write response to log file;

}

void ParallelRunGames(vector<Params> a, size_t n ) {
    parallel_for( blocked_range<size_t>(0,n),
        [=](const blocked_range<size_t>& r) {
            for(size_t i=r.begin(); i!=r.end(); ++i)
                RunGame(a[i]);
            }
    );
}

void SerialRunGames(vector<Params> a) {
    for (int i=0; i<a.size(); i++) {
        RunGame(a[i]);
    }
}


int main() {
    std::vector<double> v;

    for (auto i = 0; i < 200; i++) {
        v.push_back(i);
    }
    cfg c = cfg();
    vector<Params> cart = c.make_all_params();


    auto start = std::chrono::system_clock::now();
    std::time_t start_time = std::chrono::system_clock::to_time_t(start);
    std::cout << "started " << std::to_string(cart.size()) << " games at " << std::ctime(&start_time);

    // ParallelRunGames(cart, cart.size());
    SerialRunGames(cart);

    auto end = std::chrono::system_clock::now();
 
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
 
    std::cout << "finished computation at " << std::ctime(&end_time)
              << "elapsed time: " << elapsed_seconds.count() << "s"
              << std::endl;

    return 0;
}