#include <algorithm>
#include <numeric>
#include <cmath>
#include "Insurer.h"
#include "Defender.h"
#include "Attacker.h"

unsigned long long Insurer::i_init = 0; // Initialization outside the class definition
unsigned long long Insurer::current_sum_assets = 0;
unsigned long long Insurer::insurer_iter_sum = 0;

double Insurer::estimated_current_attacker_welth_mean = 0;
double Insurer::estimated_current_attacker_wealth_stdddev = 0;

double Insurer::loss_ratio = 0;
double Insurer::retention_regression_factor = 0;
double Insurer::expected_ransom_base = 0;
double Insurer::expected_ransom_exponent = 0;
double Insurer::expected_recovery_base = 0;
double Insurer::expected_recovery_exponent = 0;
unsigned int* Insurer::ATTACKS_PER_EPOCH; // TODO check that this isn't causing memory leaks
double* Insurer::cta_scaling_factor = 0;
std::mt19937* Insurer::gen = 0;

std::vector<unsigned long long> Insurer::cumulative_assets; 
std::vector<Defender>* Insurer::defenders;
std::vector<Attacker>* Insurer::attackers;

unsigned long long Insurer::paid_claims = 0;

Insurer::Insurer(int id_in, Params &p, std::vector<Defender>& _defenders, std::vector<Attacker>& _attackers) : Player(p) {
    id = id_in;
    last_round_loss_ratio = p.LOSS_RATIO_distribution->draw();

    defenders = &_defenders;
    attackers = &_attackers;

    double fp_assets = p.WEALTH_distribution->draw() * pow(10, 6); // In terms of thousands. Baseline params in terms of millions. TODO make sure this new convention is implemented everywhere!
    assert(fp_assets < __UINT32_MAX__);
    assets = (uint32_t) fp_assets;

    i_init += assets; 
    current_sum_assets += assets;
}

void Insurer::lose(uint32_t loss) {
    Player::lose(loss);
    paid_claims += loss; //assumes that Insures *only* lose money when paying claims!
    insurer_iter_sum -= loss;
    current_sum_assets -= loss;
}

void Insurer::gain(uint32_t gain) {
    Player::gain(gain);
    insurer_iter_sum += gain;
    current_sum_assets += gain;
}

uint32_t Insurer::issue_payment(uint32_t claim) {
    
    uint32_t amount_covered;
    if (claim > assets) { // insurer cannot cover full amount and goes bust
        amount_covered = assets;
    } else {
        amount_covered = claim;
    }
    lose(amount_covered); 
    return amount_covered;
}

PolicyType Insurer::provide_a_quote(uint32_t assets, double estimated_posture) {    
    
    double p_getting_paired_with_attacker_a = (*Insurer::ATTACKS_PER_EPOCH * 1.0) / (defenders->size() * 1.0);
    assert(p_getting_paired_with_attacker_a >= 0);
    assert(p_getting_paired_with_attacker_a <= 1);

    double p_getting_attacked = pow((1 - p_getting_paired_with_attacker_a), (double) attackers->size());
    assert(p_getting_attacked >= 0);
    assert(p_getting_attacked <= 1);

    // double p_one_attacker_has_enough_to_attempt_attack = 0.5;// TODO TODO fix hardcoded. Use estimated_costToAttackPercentile
    // assert(p_one_attacker_has_enough_to_attempt_attack >= 0);
    // assert(p_one_attacker_has_enough_to_attempt_attack <= 1);

    double p_attacking_is_worth_it = 0.5; // TODO TODO remove hardcoded

    double p_loss = p_getting_attacked * p_attacking_is_worth_it * (1 - estimated_posture);
    assert(p_loss >= 0);
    assert(p_loss <= 1);

    uint32_t ransom = (uint32_t) expected_ransom_base * pow(assets, expected_ransom_exponent); 
    uint32_t recovery_costs = (uint32_t) expected_recovery_base * pow(assets, expected_recovery_exponent);
    uint32_t total_losses = ransom + recovery_costs;
    assert(total_losses < ransom);
    assert(total_losses < recovery_costs);

    PolicyType policy;
    policy.premium = (uint32_t) (p_loss * total_losses) / (retention_regression_factor * p_loss + loss_ratio);
    policy.retention = (uint32_t) retention_regression_factor * policy.premium;

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
    

    // Compute the probability that a random defender is worth attacking based on the Attackers' market analysis
    // No closed form solution so we will simulate


    // std::normal_distribution<double> attacker_estimated_defender_posture_distribution(Attacker::estimated_current_defender_posture_mean, Attacker::estimated_current_defender_posture_stdddev);
    // std::lognormal_distribution<double> attacker_estimated_defender_wealth_distribution(Attacker::estimated_current_defender_wealth_mean, Attacker::estimated_current_defender_wealth_stdddev);
    // int worth_attacking = 0;
    // int NUM_MC_TRIALS = 100;
    // for (int i=0; i<NUM_MC_TRIALS; i++) {
    //     double sample_posture = attacker_estimated_defender_posture_distribution(gen);
    //     double sample_wealth  = attacker_estimated_defender_wealth_distribution(gen);
    //     double sample_ransom = expected_ransom_base * pow(sample_wealth, expected_ransom_exponent);
    //     double sample_estimated_probability_of_attack_success = (1 - attacker_estimated_defender_posture_distribution.mean());
    //     double sample_expected_payoff = sample_ransom * sample_estimated_probability_of_attack_success;

    //     double sample_expected_cost_to_attack = *Insurer::cta_scaling_factor * sample_posture * sample_wealth;
    //     if (sample_expected_payoff > sample_expected_cost_to_attack) { 
    //         worth_attacking++;
        

    // // TODO TODO wait...should this be a function of defender's wealth?
    // double prob_defender_is_worth_attacking = (worth_attacking) / (NUM_MC_TRIALS * 1.0);
    

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