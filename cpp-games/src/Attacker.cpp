#include "Attacker.h"

double Attacker::inequality_ratio = 0;

long long Attacker::a_init = 0; 
long long Attacker::attacker_iter_sum = 0;
long long Attacker::current_sum_assets = 0;
std::vector<unsigned long long> Attacker::cumulative_assets; 

double Attacker::estimated_current_defender_wealth_mean = 0;
double Attacker::estimated_current_defender_wealth_stdddev = 0;
double Attacker::estimated_current_defender_posture_mean = 0; /// TODO undo!!
double Attacker::estimated_current_defender_posture_stdddev = 0;

long long Attacker::attacksAttempted = 0;
long long Attacker::attacksSucceeded = 0;
long long Attacker::attackerExpenditures = 0;
long long Attacker::attackerLoots = 0; 

Attacker::Attacker(int id_in, Params &p) : Player(p) {
    id = id_in;
    double inequality_ratio = p.INEQUALITY_distribution->draw();
    assert(inequality_ratio > 0);
    assert(inequality_ratio <= 1);
    
    double fp_assets = p.WEALTH_distribution->draw() * pow(10, 6) * inequality_ratio; // In terms of thousands. Baseline params in terms of millions. TODO make sure this new convention is implemented everywhere!
    assert(fp_assets < __UINT32_MAX__);
    assets = (uint32_t) fp_assets;

    a_init += assets; 
    current_sum_assets += assets;
}

// // Function to compute the mean of a vector
// // TODO it takes a vector of doubles but assets are ints?
// // TODO does this work for lognormal data........ TODO TODO
// double computeMean(const std::vector<double>& data) {
//     double sum = 0.0;
//     for (const auto& value : data) {
//         sum += value;
//     }
//     return sum / data.size();
// }

// // Function to compute the variance of a vector
// double computeVariance(const std::vector<double>& data, double mean) {
//     double sumSquaredDiff = 0.0;
//     for (const auto& value : data) {
//         sumSquaredDiff += (value - mean) * (value - mean);
//     }
//     return sumSquaredDiff / data.size();
// }

void Attacker::perform_market_analysis() {
    // estimate estimated_current_defender_posture_mean
    Attacker::estimated_current_defender_posture_mean = 0.28; // TODO implement
}

void Attacker::lose(uint32_t loss) {
    Player::lose(loss);
    attacker_iter_sum -= loss;
    current_sum_assets -= loss;
}

void Attacker::gain(uint32_t gain) {
    Player::gain(gain);
    attacker_iter_sum += gain;
    current_sum_assets += gain;
    attackerLoots += gain; // assumes that attackers ONLY gain by looting
}

void Attacker::reset() {

    a_init = 0;
    attacker_iter_sum = 0; // how much the attackers have cumulatively gained or lost this round
    current_sum_assets = 0; // sum total of all class instances
    attacksAttempted = 0;
    attacksSucceeded = 0;
    attackerExpenditures = 0;
    attackerLoots = 0;
    cumulative_assets.clear(); // running total of all attackers' assets
}