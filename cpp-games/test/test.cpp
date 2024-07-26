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
    
    // Test cases from Mathematic (secinvestments.nb)
    d.assets = 3600000;
    Defender::estimated_probability_of_attack = 0.01;
    Defender::ransom_b0 = p.RANSOM_B0_distribution->mean();
    Defender::ransom_b1 = p.RANSOM_B1_distribution->mean();
    Defender::recovery_base = p.RECOVERY_COST_BASE_distribution->mean();
    Defender::recovery_exp  = p.RECOVERY_COST_EXP_distribution->mean();
    d.capex = 0;
    int min = (int) d.gsl_find_minimum();
    ASSERT_EQ(min, 0);

    d.assets = 1 * pow(10,7);
    Defender::estimated_probability_of_attack = 1;
    d.capex = 0;
    d.posture = 0;
    min = std::round(d.gsl_find_minimum());
    // cout << "x = 0: " << Defender::expected_loss(0, d.assets, d.capex) << endl;
    // cout << "x = 160155: " << Defender::expected_loss(160155, d.assets, d.capex) << endl;
    // cout << "x = 360155: " << Defender::expected_loss(360155, d.assets, d.capex) << endl;
    // cout << "x = 560155: " << Defender::expected_loss(560155, d.assets, d.capex) << endl;
    // cout << "x = 560165: " << Defender::expected_loss(560165, d.assets, d.capex) << endl;
    // cout << "x = 560175: " << Defender::expected_loss(560175, d.assets, d.capex) << endl;
    // cout << "x = 560185: " << Defender::expected_loss(560185, d.assets, d.capex) << endl;
    // cout << "x = 560195: " << Defender::expected_loss(560195, d.assets, d.capex) << endl;
    // cout << "x = 560205: " << Defender::expected_loss(560205, d.assets, d.capex) << endl;
    // cout << "x = 760175: " << Defender::expected_loss(760175, d.assets, d.capex) << endl;
    // cout << "x = 1000000: " << Defender::expected_loss(1000000, d.assets, d.capex) << endl;
    // cout << "x = assets: " << Defender::expected_loss(d.assets, d.assets, d.capex) << endl;


    ASSERT_EQ(Defender::expected_loss(min, d.assets, d.capex),  Defender::expected_loss(560175, d.assets, d.capex));

    d.assets = 1 * pow(10,6);
    Defender::estimated_probability_of_attack = 0.8;
    d.capex = 1000;
    d.posture = 0.3;
    min = std::round(d.gsl_find_minimum());
    ASSERT_EQ(Defender::expected_loss(min, d.assets, d.capex),  Defender::expected_loss(69615, d.assets, d.capex));

}

TEST(MCSGtest, SweepDistributionTest) {
    Defender::reset();
    string filename = "../configs/fullsize_sweep_NUM_QUOTES.json";
    Params p = params_loader::load_cfg(filename);
    SweepDistribution* s = dynamic_cast<SweepDistribution*>(p.NUM_QUOTES_distribution);

    // double min = s->min;
    // double max = s->max;
    // double step = s->step;

    double next_step = s->min;
    while (next_step <= s->max) {
        double draw = s->draw();
        // cout << next_step << " == " << draw << " ? " << (next_step == draw ? "PASS" : "FAIL") << endl;
        ASSERT_FLOAT_EQ(next_step, draw);
        next_step += s->step;
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}