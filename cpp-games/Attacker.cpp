#include "Attacker.h"


uint Attacker::a_init = 0; 
double Attacker::attacker_iter_sum = 0;
double Attacker::current_sum_assets = 0;
std::vector<double> Attacker::cumulative_assets; 

int Attacker::attacksAttempted = 0;
int Attacker::attacksSucceeded = 0;
int Attacker::attackerExpenditures = 0;
int Attacker::attackerLoots = 0; 


Attacker::Attacker(int id_in, Params &p) : Player(p) {
    id = id_in;

    assets = p.WEALTH_distribution->draw() * p.INEQUALITY; // TODO check this...this doesn't need to a distribution, right?
    if (assets < 0) {
        assets = 0;
    }

    a_init += assets; // TODO should this be a static class variable?
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