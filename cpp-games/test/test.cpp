#include <iostream>
#include <string>
#include <gtest/gtest.h>
#include "../src/Defender.h"
#include "../src/params.h"
using namespace std;

TEST(MCSGtest, HelloWorldTest) {
    // Your test code here
    EXPECT_EQ(1, 1);
    ASSERT_TRUE(true);
}

TEST(MCSGtest, FindInvestmentMinimumTest) {
    Defender::reset();
    int id = 0;
    string filename = "../configs/fullsize_tiny.json";
    Params p = params_loader::load_cfg(filename);
    Defender d = Defender(id, p);
    
    d.assets = 3600000;
    Defender::estimated_probability_of_attack = 0.01;
    Defender::ransom_b0 = p.RANSOM_B0_distribution->mean();
    Defender::ransom_b1 = p.RANSOM_B1_distribution->mean();
    Defender::recovery_base = p.RECOVERY_COST_BASE_distribution->mean();
    Defender::recovery_exp  = p.RECOVERY_COST_EXP_distribution->mean();
    d.capex = 0;
    int min = (int) d.gsl_find_minimum();
    ASSERT_EQ(min, 0);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}