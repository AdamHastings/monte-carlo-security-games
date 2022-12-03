#include <iostream>
#include <vector>
#include <string>
#include "oneapi/tbb.h"
#include "Player.h"
#include "Game.h"


using namespace oneapi::tbb;
using namespace std;


class cfg {
    public:
        vector<float> MANDATE_range    = {0.1};
        vector<float> ATTACKERS_range  = {0.1};
        vector<float> INEQUALITY_range = {0.1};
        vector<float> PREMIUM_range    = {0.1};
        vector<float> EFFICIENCY_range = {0.1};
        vector<float> EFFORT_range     = {0.1};
        vector<float> PAYOFF_range     = {0.1};
        vector<float> CAUGHT_range     = {0.1};
        vector<float> CLAIMS_range     = {0.1};
        vector<float> TAX_range        = {0.1};

        int B = 1000;
        int N = 10000;
        int E = 10;
        int D = 50;


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
    std::cout << "Hello world" << std::endl;

    std::vector<float> v;

    for (auto i = 0; i < 200; i++) {
        v.push_back(i);
    }
    cfg c = cfg();
    vector<Params> cart = c.make_all_params();

    ParallelRunGames(cart, cart.size());

    Player p = Player();
    cout << p.get_assets() << endl;
    p.gain(100);
    cout << p.get_assets() << endl;

    return 0;
}