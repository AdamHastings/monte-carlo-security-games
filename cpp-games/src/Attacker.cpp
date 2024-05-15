#include "Attacker.h"

double Attacker::inequality_ratio = 0;

double Attacker::a_init = 0; 
double Attacker::attacker_iter_sum = 0;
double Attacker::current_sum_assets = 0;
std::vector<double> Attacker::cumulative_assets; 

int Attacker::attacksAttempted = 0;
int Attacker::attacksSucceeded = 0;
double Attacker::attackerExpenditures = 0;
double Attacker::attackerLoots = 0; 

Attacker::Attacker(int id_in, Params &p) : Player(p) {
    id = id_in;
    double inequality_ratio = p.INEQUALITY_distribution->draw();
    assert(inequality_ratio > 0);
    assert(inequality_ratio <= 1);
    
    // parameters scaled down by 1B during curve fitting to avoid numerical overflow
    // so I re-scale back up by 1B here to compensate
    assets = p.WEALTH_distribution->draw() * pow(10, 9) * inequality_ratio;
    if (assets < 0) {
        assets = 0;
    }

    a_init += assets; 
    current_sum_assets += assets;
}

void Attacker::lose(double loss) {
    Player::lose(loss);
    attacker_iter_sum -= loss;
    current_sum_assets -= loss;
}

void Attacker::gain(double gain) {
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