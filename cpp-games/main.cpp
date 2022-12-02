#include <iostream>
#include <vector>
#include "oneapi/tbb.h"


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


        // cfg::cfg/() {} // Nothing to initialize for now

        vector<vector<float>> get_cartesian() {
            
            vector<vector<float>> ret;

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
                vector<float> next = {a,b,c,d,e,f,g,h,i,j};
                ret.push_back(next);
            }}}}}}}}}}

            return ret;
        }
};


void Foo(float x) {
    cout << x << endl;
}

void vFoo(vector<float> x) {
    cout << "( ";
    for (auto i: x){
        std::cout << i <<  " ";
    }
    cout << ")" << endl;
}


// void ParallelApplyFoo(std::vector<vector<double>> v, size_t n) {
//     parallel_for(size_t(0), n, [=](size_t i) {Foo(v[i]);});
// }

void ParallelApplyVectorFoo(vector<vector<float>> a, size_t n ) {
    parallel_for( blocked_range<size_t>(0,n),
        [=](const blocked_range<size_t>& r) {
            for(size_t i=r.begin(); i!=r.end(); ++i)
                vFoo(a[i]);
            }
    );
}

void ParallelApplyFoo(vector<float> a, size_t n ) {
    parallel_for( blocked_range<size_t>(0,n),
        [=](const blocked_range<size_t>& r) {
            for(size_t i=r.begin(); i!=r.end(); ++i)
                Foo(a[i]);
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
    // cout << c << endl;
    vector<vector<float>> cart = c.get_cartesian();
    // for (auto i: cart){
    //     cout << "(";
    //     for (auto j: i) {
    //         cout << j << " ";
    //     }
    //     cout << ")" << endl;
    // }

    // ParallelApplyFoo(cart, cart.size());
    ParallelApplyFoo(v, v.size());
    ParallelApplyVectorFoo(cart, cart.size());
    
    return 0;
}