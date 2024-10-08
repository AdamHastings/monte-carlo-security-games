#include "Attacker.h"
#include "Defender.h"
#include "utils.h"


long long Attacker::a_init = 0; 
int64_t Attacker::attacker_iter_sum = 0;
int64_t Attacker::current_sum_assets = 0;
std::vector<unsigned long long> Attacker::cumulative_assets; 

double Attacker::estimated_current_defender_posture_mean = 0; 
double Attacker::inequality_ratio = 0;

long long Attacker::attacksAttempted = 0;
long long Attacker::attacksSucceeded = 0;
long long Attacker::attackerExpenditures = 0;
long long Attacker::attackerLoots = 0; 

Attacker::Attacker(int id_in, Params &p) : Player(p) {
    id = id_in;
    assert(id >= 0);

    double inequality_ratio = p.INEQUALITY_distribution->draw();
    assert(inequality_ratio > 0);
    assert(inequality_ratio <= 1);
    
    double fp_assets = p.WEALTH_distribution->draw() * pow(10, 9) * inequality_ratio; // Baseline params in terms of billions, so we undo here 
    assert(fp_assets < __UINT64_MAX__);
    assets = (int64_t) fp_assets;

    a_init += assets; 
    current_sum_assets += assets;
}

// we can't rely on a simple proportion of numSuccessfulRoundAttacks / numRoundAttacks
// because presumably there will be valid round with no attacks
// and this doesn't mean that the estimated posture should be super high.
// So we do Method of Moments
void Attacker::perform_market_analysis(std::vector<Defender> &defenders) {
    std::vector<double> defender_postures;
    for (uint i=0; i<defenders.size(); i++) {
        if (defenders[i].is_alive()) {
            defender_postures.push_back(defenders[i].posture);
        }
    }

    double sampleMean = utils::computeMean(defender_postures);
    estimated_current_defender_posture_mean = sampleMean; 
}

void Attacker::lose(int64_t loss) {
    Player::lose(loss);
    attacker_iter_sum -= loss;
    current_sum_assets -= loss;
}

void Attacker::gain(int64_t gain) {
    Player::gain(gain);
    attacker_iter_sum += gain;
    current_sum_assets += gain;
    attackerLoots += gain; // assumes that attackers ONLY gain by looting
}

void Attacker::reset() {
    a_init = 0;
    attacker_iter_sum = 0; // how much the attackers have cumulatively gained or lost this round
    current_sum_assets = 0; // sum total of all class instances
    cumulative_assets.clear(); // running total of all attackers' assets

    estimated_current_defender_posture_mean = 0; 
    inequality_ratio = 0;
    
    attacksAttempted = 0;
    attacksSucceeded = 0;
    attackerExpenditures = 0;
    attackerLoots = 0;
}