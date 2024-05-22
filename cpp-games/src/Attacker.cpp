#include "Attacker.h"
#include "Defender.h"
#include "utils.h"

double Attacker::inequality_ratio = 0;

long long Attacker::a_init = 0; 
long long Attacker::attacker_iter_sum = 0;
long long Attacker::current_sum_assets = 0;
std::vector<unsigned long long> Attacker::cumulative_assets; 

// double Attacker::estimated_current_defender_wealth_mean = 0;
// double Attacker::estimated_current_defender_wealth_stdddev = 0;
double Attacker::estimated_current_defender_posture_mean = 0; 
// double Attacker::estimated_current_defender_posture_stdddev = 0;

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

void Attacker::perform_market_analysis(std::vector<Defender> &defenders) {
    // we can't rely on a simple proportion of numSuccessfulRoundAttacks / numRoundAttacks
    // because presumably there will be valid round with no attacks
    // and this doesn't mean that the estimated posture should be super high.
    // So we do MOM

    std::vector<double> defender_postures;

    for (uint i=0; i<defenders.size(); i++) {
        if (defenders[i].alive) {
            defender_postures.push_back(defenders[i].posture);
        }
    }

    // Compute sample mean and variance
    double sampleMean = utils::computeMean(defender_postures);
    // double sampleVariance = utils::computeVariance(defender_postures, sampleMean);

    // // Compute parameters using method of moments
    // double variance_mom = log(1 + pow(sampleVariance, 2) / pow(sampleMean, 2));
    // double mu_mom = log(sampleMean) - 0.5 * variance_mom;
    // double sigma_mom = sqrt(variance_mom);

    // estimated_current_defender_wealth_mean    = mu_mom;
    // estimated_current_defender_wealth_stdddev = sigma_mom;

    estimated_current_defender_posture_mean = sampleMean;
    // estimated_current_defender_posture_stdddev
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