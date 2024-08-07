#include <iostream>
#include <string>
#include <gtest/gtest.h>
#include <iterator> // for iterators 
// #include <sstream>
#include <cstdlib> // For system()
#include <iomanip> // For std::scientific and std::setprecision
#include "../src/json/json.h" // You can use any JSON library
#include "../src/Defender.h"
#include "../src/params.h"
#include "../src/utils.h"
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

// TEST(MCSGtest, SweepDistributionTest) {
//     Defender::reset();
//     string filename = "../configs/fullsize_sweep_NUM_QUOTES.json";
//     Params p = params_loader::load_cfg(filename);
//     SweepDistribution* s = dynamic_cast<SweepDistribution*>(p.NUM_QUOTES_distribution);

//     // double min = s->min;
//     // double max = s->max;
//     // double step = s->step;

//     double next_step = s->min;
//     while (next_step <= s->max) {
//         double draw = s->draw();
//         // cout << next_step << " == " << draw << " ? " << (next_step == draw ? "PASS" : "FAIL") << endl;
//         ASSERT_FLOAT_EQ(next_step, draw);
//         next_step += s->step;
//     }
// }

// TEST(MCSGtest, HighMandatoryInvestmentTest) {
//     std::vector<double> scaled_attacker_assets = {
//         0.57643500000000003, 
//         0.093563999999999994, 
//         0.0047790000000000003, 
//         0.160549, 
//         0.065795000000000006
//     };

//     double scaled_mu =  utils::compute_mu_mom_lognormal(scaled_attacker_assets); // Note!! This is the log of the median, and mean  is exp(mu + (sigma^2)/2)!! Staying in terms of paramters mu and sigma
//     ASSERT_EQ(scaled_mu, 0);
// }

// std::string vector_to_json(const std::vector<int64_t>& vec) {
//     Json::Value jsonArray;
//     for (int64_t num : vec) {
//         jsonArray.append(num);
//     }
//     Json::StreamWriterBuilder writer;
//     return Json::writeString(writer, jsonArray);
// }


TEST(MCSGtest, WealthGen) {
    Defender::reset();
    int id = 0;
    string filename = "../configs/fullsize_tiny.json";
    Params p = params_loader::load_cfg(filename);   

    vector<int64_t> wealths;
    for (int i=0; i < p.NUM_DEFENDERS_distribution->mean(); i++) {
        Defender d = Defender(id, p);
        // std::cout << static_cast<double>(d.assets) << std::scientific << "  " << std::setprecision(2);
        wealths.push_back(d.assets);
    }
    cout << endl;
    // string str(wealths.begin(), wealths.end());
    // cout << str << endl;

    stringstream vecstr;
    bool first = true;
    vector<int64_t>::iterator begin = wealths.begin();
    vector<int64_t>::iterator end = wealths.end();
    for (; begin != end; begin++)
    {
        if (!first)
            vecstr << ",";
        vecstr << *begin;
        first = false;
    }

    // ASSERT sum is less than something reasonably big

    // std::string vectorJson = vector_to_json(wealths);
    // string command = "python3 ../scripts/plot_distribution.py defender_wealths '" + vectorJson + "'";
    string command = "python3 ../scripts/plot_distribution.py defender_wealths " + vecstr.str();
    // cout << command << endl;

    // Execute the command
    int result = system(command.c_str());

    if (result == 0) {
        std::cout << "Python script executed successfully." << std::endl;
    } else {
        std::cerr << "Error executing Python script." << std::endl;
    }

}



int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}