#include <iostream>
#include <vector>
#include "oneapi/tbb.h"


using namespace oneapi::tbb;


void Foo(float x) {
    std::cout << x << std::endl;
}


void ParallelApplyFoo(std::vector<float> v, size_t n) {
    parallel_for(size_t(0), n, [=](size_t i) {Foo(v[i]);});
}


int main() {
    std::cout << "Hello world" << std::endl;

    std::vector<float> v;

    for (auto i = 0; i < 200; i++) {
        v.push_back(i);
    }
    ParallelApplyFoo(v, v.size());
    return 0;
}