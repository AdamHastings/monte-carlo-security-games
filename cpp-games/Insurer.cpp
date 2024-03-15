#include <algorithm>
#include "Insurer.h"
#include "Defender.h"

uint Insurer::i_init = 0; // Initialization outside the class definition
double Insurer::current_sum_assets = 0;
double Insurer::insurer_iter_sum = 0;
std::vector<double> Insurer::cumulative_assets; 
std::vector<Defender>* Insurer::defenders;
std::vector<Attacker>* Insurer::attackers;

double Insurer::paid_claims = 0;

Insurer::Insurer(int id_in, Params &p, std::vector<Defender>& _defenders, std::vector<Attacker>& _attackers) : Player(p) {
    id = id_in;

    defenders = &_defenders;
    attackers = &_attackers;

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



    double probability_of_getting_paried_with_attacker = std::min(1.0, ((attackers->size() * 1.0) / (defenders->size() * 1.0)));
    double probability_random_attacker_has_enough_to_attack = (1 - estimated_costToAttackPercentile);
    double p_A = probability_of_getting_paried_with_attacker * probability_random_attacker_has_enough_to_attack;
    double p_L = p_A * (1 - estimated_posture); 
    double mean_PAYOFF = p.PAYOFF_distribution->mean();

    std::cout << probability_of_getting_paried_with_attacker << " " << probability_random_attacker_has_enough_to_attack << " " << p_A << " " << p_L << " " << mean_PAYOFF << std::endl;
    
    policy.premium = (p_L * mean_PAYOFF * assets) / (r * p_L + OVerhead);
    policy.retention = r * policy.premium;


    return policy;
}

double Insurer::findPercentile(const std::vector<double>& sortedVector, double newValue) {
    // Find the rank of the new value within the sorted vector
    auto rankIterator = std::lower_bound(sortedVector.begin(), sortedVector.end(), newValue);
    int rank = std::distance(sortedVector.begin(), rankIterator);
    
    // Calculate the percentile
    double percentile = (static_cast<double>(rank) / (sortedVector.size() - 1)) * 100.0;
    
    return percentile;
}

// Insurers use their overhead to conduct operations and perform risk analysis
// As part of this, the insurers find the median assets of the attackers (TODO maybe estimate it even?)
// which informs current defender risks before writing policies.
void Insurer::perform_market_analysis(int prevRoundAttacks){
    
    std::vector<double> attacker_assets;
    for (auto a = attackers->begin(); a != attackers->end(); ++a) {

        if (a->alive) {
            attacker_assets.push_back(a->assets);
        }
    }

    std::sort(attacker_assets.begin(), attacker_assets.end());


    for (auto d = defenders->begin(); d != defenders->end(); ++d) {
        d->costToAttackPercentile = findPercentile(attacker_assets, d->costToAttack); // TODO this is returning crazy values and likely the source of the crashes.
    }

    // Defenders don't have the same visibility as the insurers but still can make some predictions about risk.
    Defender::estimated_probability_of_attack = std::min(1.0, (prevRoundAttacks * 1.0)/(defenders->size() * 1.0));
}