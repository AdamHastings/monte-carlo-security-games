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
        // vector<float> MANDATE_range    = {0.1, 0.2, 0.3, 0.4, 0.5};
        // vector<float> ATTACKERS_range  = {0.1, 0.2};
        // vector<float> INEQUALITY_range = {0.1};
        // vector<float> PREMIUM_range    = {0.1, 0.9};
        // vector<float> EFFICIENCY_range = {0.1};
        // vector<float> EFFORT_range     = {0.1};
        // vector<float> PAYOFF_range     = {0.1};
        // vector<float> CAUGHT_range     = {0.1};
        // vector<float> CLAIMS_range     = {0.1};
        // vector<float> TAX_range        = {0.1};

        // vector<float> MANDATE_range    = {0.1, 0.5, 0.9};
        // vector<float> ATTACKERS_range  = {0.1, 0.5, 1.0};
        // vector<float> INEQUALITY_range = {0.1, 0.5, 1.0};
        // vector<float> PREMIUM_range    = {0.0, 0.5, 1.0};
        // vector<float> EFFICIENCY_range = {0.1, 0.5, 1.0};
        // vector<float> EFFORT_range     = {0.1, 0.5, 1.0};
        // vector<float> PAYOFF_range     = {0.1, 0.5, 1.0};
        // vector<float> CAUGHT_range     = {0.1, 0.5, 1.0};
        // vector<float> CLAIMS_range     = {0.1, 0.5, 1.0};
        // vector<float> TAX_range        = {0.1, 0.5, 1.0};

        // test_small
        vector<float> MANDATE_range    = {0.1, 0.8};
        vector<float> ATTACKERS_range  = {0.5, 1.0};
        vector<float> INEQUALITY_range = {0.3, 1.0};
        vector<float> PREMIUM_range    = {0.0, 0.5, 1.0};
        vector<float> EFFICIENCY_range = {0.1, 1.0};
        vector<float> EFFORT_range     = {0.1, 0.4};
        vector<float> PAYOFF_range     = {0.3, 0.9};
        vector<float> CAUGHT_range     = {0.1, 0.6};
        vector<float> CLAIMS_range     = {0.1, 1.0};
        vector<float> TAX_range        = {0.0, 0.5, 1.0};

        int B = 1000;
        int N = 10000;
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
    std::vector<Defender> defenders;
    for (int i=0; i < p.B; i++) {
        Defender d = Defender();

        float investment = d.assets * p.MANDATE;
        float selfless_investment = investment * p.TAX;
        float selfish_investment  = investment - selfless_investment;

        float tax = selfless_investment;
        d.lose(tax);
        government.gain(tax);

        float insurance = selfless_investment * p.PREMIUM;
        d.lose(insurance);
        insurer.gain(insurance);

        float personal_security_investment = selfish_investment - insurance;
        d.costToAttack = d.assets * p.EFFORT;
        d.costToAttack += personal_security_investment * p.EFFORT;

        defenders.push_back(Defender(d));
    }

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


int main() {
    std::vector<float> v;

    for (auto i = 0; i < 200; i++) {
        v.push_back(i);
    }
    cfg c = cfg();
    vector<Params> cart = c.make_all_params();


    auto start = std::chrono::system_clock::now();
    std::time_t start_time = std::chrono::system_clock::to_time_t(start);
    std::cout << "started " << std::to_string(cart.size()) << " games at " << std::ctime(&start_time);

    ParallelRunGames(cart, cart.size());

    auto end = std::chrono::system_clock::now();
 
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
 
    std::cout << "finished computation at " << std::ctime(&end_time)
              << "elapsed time: " << elapsed_seconds.count() << "s"
              << std::endl;

    return 0;
}