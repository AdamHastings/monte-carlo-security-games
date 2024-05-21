#include <algorithm>
#include <numeric>
#include <cmath>
#include "Insurer.h"
#include "Defender.h"

double Insurer::i_init = 0; // Initialization outside the class definition
double Insurer::current_sum_assets = 0;
double Insurer::insurer_iter_sum = 0;

double Insurer::estimated_current_attacker_welth_mean = 0;
double Insurer::estimated_current_attacker_wealth_stdddev = 0;

double Insurer::loss_ratio = 0;
double Insurer::retention_regression_factor = 0;
double Insurer::expected_ransom_base = 0;
double Insurer::expected_ransom_exponent = 0;
double Insurer::expected_recovery_base = 0;
double Insurer::expected_recovery_exponent = 0;
unsigned int* Insurer::ATTACKS_PER_EPOCH; // TODO check that this isn't causing memory leaks
double* Insurer::cta_scaling_factor;


std::vector<double> Insurer::cumulative_assets; 
std::vector<Defender>* Insurer::defenders;
std::vector<Attacker>* Insurer::attackers;

double Insurer::paid_claims = 0;

Insurer::Insurer(int id_in, Params &p, std::vector<Defender>& _defenders, std::vector<Attacker>& _attackers) : Player(p) {
    id = id_in;
    last_round_loss_ratio = p.LOSS_RATIO_distribution->draw();

    defenders = &_defenders;
    attackers = &_attackers;

    assets = p.WEALTH_distribution->draw() * pow(10, 9);
    if (assets < 0) {
        assets = 0;
    }

    i_init += assets; 
    current_sum_assets += assets;
}

void Insurer::lose(double loss) {
    Player::lose(loss);
    paid_claims += loss; //assumes that Insures *only* lose money when paying claims!
    insurer_iter_sum -= loss;
    current_sum_assets -= loss;
}

void Insurer::gain(double gain) {
    Player::gain(gain);
    insurer_iter_sum += gain;
    current_sum_assets += gain;
}

double Insurer::issue_payment(double claim) {
    
    double amount_covered;
    if (claim > assets) { // insurer cannot cover full amount and goes bust
        amount_covered = assets;
    } else {
        amount_covered = claim;
    }
    lose(amount_covered); 
    return amount_covered;
}

PolicyType Insurer::provide_a_quote(double assets, double estimated_posture, double estimated_costToAttackPercentile) {    
    
    double p_getting_paired_with_attacker_a = (*Insurer::ATTACKS_PER_EPOCH * 1.0) / (defenders->size() * 1.0);
    assert(p_getting_paired_with_attacker_a >= 0);
    assert(p_getting_paired_with_attacker_a <= 1);

    double p_getting_attacked = pow((1 - p_getting_paired_with_attacker_a), (double) attackers->size());
    assert(p_getting_attacked >= 0);
    assert(p_getting_attacked <= 1);

    double p_one_attacker_has_enough_to_attempt_attack = 0.5;// TODO TODO fix hardcoded. Use estimated_costToAttackPercentile
    assert(p_one_attacker_has_enough_to_attempt_attack >= 0);
    assert(p_one_attacker_has_enough_to_attempt_attack <= 1);

    double p_loss = p_getting_attacked * p_one_attacker_has_enough_to_attempt_attack * (1 - estimated_posture);
    assert(p_loss >= 0);
    assert(p_loss <= 1);

    double ransom = expected_ransom_base * pow(assets, expected_ransom_exponent); 
    double recovery_costs = expected_recovery_base * pow(assets, expected_recovery_exponent);
    double total_losses = ransom + recovery_costs;

    PolicyType policy;
    policy.premium = (p_loss * total_losses) / (retention_regression_factor * p_loss + loss_ratio);
    policy.retention = retention_regression_factor * policy.premium;

    assert(policy.premium > 0); // I'd like to not have to consider cases where premium = 0
    assert(policy.retention > 0);

    return policy;
}

// double Insurer::findPercentile(const std::vector<double>& sortedVector, double newValue) {
//     // Find the rank of the new value within the sorted vector
//     auto rankIterator = std::lower_bound(sortedVector.begin(), sortedVector.end(), newValue);
//     int rank = std::distance(sortedVector.begin(), rankIterator);
    
//     // Calculate the percentile
//     double percentile = (static_cast<double>(rank) / ((sortedVector.size() + 1) * 1.0));
//     assert(percentile >= 0);
//     assert(percentile <= 1);
    
//     return percentile;
// }


// Function to compute the mean of a vector
double computeMean(const std::vector<double>& data) {
    double sum = 0.0;
    for (const auto& value : data) {
        sum += value;
    }
    return sum / data.size();
}

// Function to compute the variance of a vector
double computeVariance(const std::vector<double>& data, double mean) {
    double sumSquaredDiff = 0.0;
    for (const auto& value : data) {
        sumSquaredDiff += (value - mean) * (value - mean);
    }
    return sumSquaredDiff / data.size();
}

// Insurers use their overhead to conduct operations and perform risk analysis
// As part of this, the insurers find the median assets of the attackers (TODO maybe estimate it even?)
// which informs current defender risks before writing policies.
void Insurer::perform_market_analysis(int prevRoundAttacks){
    
    // TODO TODO TODO should Insurers lose 20% of their assets each round as part of operating overhead?


    std::vector<double> attacker_assets;

    for (auto a = attackers->begin(); a != attackers->end(); ++a) {
        if (a->alive) {
            attacker_assets.push_back(a->assets);
        }
    }

    // Compute sample mean and variance
    double sampleMean = computeMean(attacker_assets);
    double sampleVariance = computeVariance(attacker_assets, sampleMean);

    // Compute parameters using method of moments
    double mu_mom = log(sampleMean) - 0.5 * log(1 + sampleVariance / (sampleMean * sampleMean));
    double sigma_mom = sqrt(log(1 + sampleVariance / (sampleMean * sampleMean)));

    // TODO check that working in terms of Billions of assets is not causing numerical overflow

    estimated_current_attacker_welth_mean     = mu_mom;
    estimated_current_attacker_wealth_stdddev = sigma_mom;
    

    // Just compute policies when getting a quote so that we can capture variations in sampling of posture 
    // for (auto d = defenders->begin(); d != defenders->end(); ++d) {
    //     // d->costToAttackPercentile = findPercentile(attacker_assets, d->costToAttack);
    //     double cdf_d =  0.5 * (1 + erf((log(cta) - mu_mom) / (sigma_mom * sqrt(2))));;
    //     if (cdf_d >= 0.99) {
    //         cdf_d = 0.99;
    //     }
    //     assert(cdf_d <= 1);
    //     assert(cdf_d >= 0);
    //     d->costToAttack
    // }

    // Defenders don't have the same visibility as the insurers but still can make some predictions about risk.
    // TODO move to Defender::perform_market_analysis
    // Defender::estimated_probability_of_attack = std::min(1.0, (prevRoundAttacks * 1.0)/(defenders->size() * 1.0));
    // assert(Defender::estimated_probability_of_attack >= 0);
    // assert(Defender::estimated_probability_of_attack <= 1);
}

void Insurer::reset(){
    i_init = 0;
    insurer_iter_sum = 0;
    current_sum_assets = 0;
    paid_claims = 0;
    defenders = nullptr;
    attackers = nullptr;
    cumulative_assets.clear();
}