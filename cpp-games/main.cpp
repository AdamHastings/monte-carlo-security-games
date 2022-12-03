#include <iostream>
#include <vector>
#include <string>
#include "oneapi/tbb.h"
#include "Player.h"


using namespace oneapi::tbb;
using namespace std;


class cfg {
    public:
        vector<float> MANDATE_range    = {0.1, 0.9};
        vector<float> ATTACKERS_range  = {0.1, 0.9};
        vector<float> INEQUALITY_range = {0.1, 0.9};
        vector<float> PREMIUM_range    = {0.1, 0.9};
        vector<float> EFFICIENCY_range = {0.1, 0.9};
        vector<float> EFFORT_range     = {0.1, 0.9};
        vector<float> PAYOFF_range     = {0.1, 0.9};
        vector<float> CAUGHT_range     = {0.1, 0.9};
        vector<float> CLAIMS_range     = {0.1, 0.9};
        vector<float> TAX_range        = {0.1, 0.9};

        int B = 1000;
        int N = 10000;
        int epsilon = 10;
        int delta = 50;


        // cfg::cfg/() {} // Nothing to initialize for now

        vector<map<std::string, float>> get_cartesian() {
            
            vector<map<std::string, float>> ret;

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
                std::map<std::string, float> next;
                next["MANDATE"]    = a;
                next["ATTACKERS"]  = b;
                next["INEQUALITY"] = c;
                next["PREMIUM"]    = d;
                next["EFFICIENCY"] = e;
                next["EFFORT"]     = f;
                next["PAYOFF"]     = g;
                next["CAUGHT"]     = h;
                next["CLAIMS"]     = i;
                next["TAX"]        = j;

                next["B"]          = B;
                next["N"]          = N;
                next["delta"]      = delta;
                next["epsilon"]    = epsilon;
                ret.push_back(next);
            }}}}}}}}}}

            return ret;
        }
};

void RunGame(map<std::string, float> params) {
    
    Insurer insurer = Insurer();
    Government goverment = Government();

    // TODO put a lot of this into constructor?
    // And pass in Insurer and Government as pointers.
    std::vector<Defender> defenders;
    for (int i=0; i < params["B"]; i++) {
        Defender d = Defender();

        float investment = d.assets * params["MANDATE"];
        float selfless_investment = investment * params["TAX"];
        float selfish_investment  = investment - selfless_investment;

        float tax = selfless_investment;
        d.lose(tax);
        goverment.gain(tax);

        float insurance = selfless_investment * params["PREMIUM"];
        d.lose(insurance);
        insurer.gain(insurance);

        float personal_security_investment = selfish_investment - insurance;
        d.costToAttack = d.assets * params["EFFORT"];
        d.costToAttack += personal_security_investment * params["EFFORT"];

        defenders.push_back(Defender(d));
    }

    std::vector<Attacker> attackers;
    for (int i=0; i < params["B"] * params["ATTACKERS"]; i++) {
        Attacker a = Attacker(params["INEQUALITY"]);
        attackers.push_back(a);
    }
}

void ParallelRunGames(vector<map<std::string, float>> a, size_t n ) {
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
    vector<map<std::string, float>> cart = c.get_cartesian();

    ParallelRunGames(cart, cart.size());

    Player p = Player();
    cout << p.get_assets() << endl;
    p.gain(100);
    cout << p.get_assets() << endl;

    return 0;
}