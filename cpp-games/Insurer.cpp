
#include "Insurer.h"

uint Insurer::i_init = 0; // Initialization outside the class definition
double Insurer::current_sum_assets = 0;
double Insurer::insurer_iter_sum = 0;
std::vector<double> Insurer::cumulative_assets; 

double Insurer::paid_claims = 0;

// Should be set each iteration by the game. 
int Insurer::num_defenders = 0;
int Insurer::num_attackers = 0;
std::vector<double> Insurer::attacker_assets; // TODO is this redundant with cumulative_Assets?


Insurer::Insurer(int id_in, Params &p) : Player(p) {
    id = id_in;

    assets = p.WEALTH_distribution->draw();
    if (assets < 0) {
        assets = 0;
    }

    i_init += assets; 
}

void Insurer::lose(double loss) {
    Player::lose(loss);
    insurer_iter_sum -= loss;
    current_sum_assets -= loss;
}

void Insurer::gain(double gain) {
    Player::gain(gain);
    insurer_iter_sum += gain;
    current_sum_assets += gain;
}

double Insurer::issue_payment(double claim) {
    
    double amount_covered = 0;
    if (amount_covered > assets) {
        // insurer cannot cover full amount
        amount_covered = assets;
        // insurerTimesOfDeath.push_back(iter_num); // TODO put back in later

    } else {
        amount_covered = claim;
    }
    lose(amount_covered);
    return amount_covered;
}

PolicyType Insurer::provide_a_quote(double assets, double estimated_posture, double estimated_costToAttackPercentile) {    
    PolicyType policy;

    // TODO make these const static class vars
    double OVerhead = 0.20; // 20% overhead // Better to call this a "loss ratio" actually (standard terminology for insurance)
    double r = 20.0; // TODO double check retention regression factor



    double probability_of_getting_paried_with_attacker = std::min(1.0, ((num_attackers * 1.0) / (num_defenders * 1.0)));
    double probability_random_attacker_has_enough_to_attack = (1 - estimated_costToAttackPercentile);
    double p_A = probability_of_getting_paried_with_attacker * probability_random_attacker_has_enough_to_attack;
    double p_L = p_A * (1 - estimated_posture); 
    double mean_PAYOFF = p.PAYOFF_distribution->mean();

    std::cout << probability_of_getting_paried_with_attacker << " " << probability_random_attacker_has_enough_to_attack << " " << p_A << " " << p_L << " " << mean_PAYOFF << std::endl;
    
    policy.premium = (p_L * mean_PAYOFF * assets) / (r * p_L + OVerhead);
    policy.retention = r * policy.premium;


    return policy;
}