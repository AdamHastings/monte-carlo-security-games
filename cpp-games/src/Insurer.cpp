#include <algorithm>
#include <numeric>
#include <cmath>
#include "Insurer.h"
#include "Defender.h"
#include "Attacker.h"
#include "utils.h"

unsigned long long Insurer::i_init = 0; // Initialization outside the class definition
unsigned long long Insurer::current_sum_assets = 0;
unsigned long long Insurer::insurer_iter_sum = 0;

double Insurer::estimated_current_attacker_wealth_mean = 0;
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

    double fp_assets = p.WEALTH_distribution->draw() * pow(10, 6); // In terms of thousands. Baseline params in terms of billions. 
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
    
    double p_getting_paired_with_attacker_a = std::min(1.0, (*Insurer::ATTACKS_PER_EPOCH * 1.0) / (defenders->size() * 1.0));
    assert(p_getting_paired_with_attacker_a >= 0);
    assert(p_getting_paired_with_attacker_a <= 1);

    double p_getting_attacked;
    if (p_getting_paired_with_attacker_a == 1.0) {
        p_getting_attacked = 1.0;
    } else {
       p_getting_attacked = 1 - pow((1 - p_getting_paired_with_attacker_a), (double) attackers->size());      
    } 
    assert(p_getting_attacked >= 0);
    assert(p_getting_attacked <= 1);

    bool attacking_expected_gains_outweigh_expected_costs = (Attacker::estimated_current_defender_posture_mean < (1.0/(1 + *cta_scaling_factor)));
    if (!attacking_expected_gains_outweigh_expected_costs) {
        std::cout << "Attacking no longer worth it!" << std::endl;
    }

    uint32_t ransom = (uint32_t) expected_ransom_base * pow(assets, expected_ransom_exponent); 
    uint32_t recovery_costs = (uint32_t) expected_recovery_base * pow(assets, expected_recovery_exponent);
    uint32_t total_losses = ransom + recovery_costs;
    
    uint32_t expected_cost_to_attack = (uint32_t) (p.CTA_SCALING_FACTOR_distribution->mean() * Attacker::estimated_current_defender_posture_mean * ransom); 

    double p_one_attacker_has_enough_to_attack =  1 -  0.5 * (1 + erf((log(expected_cost_to_attack) - estimated_current_attacker_wealth_mean) / (estimated_current_attacker_wealth_stdddev * sqrt(2))));;
    assert(p_one_attacker_has_enough_to_attack >= 0);
    assert(p_one_attacker_has_enough_to_attack <= 1);

    double p_at_least_one_attacker_has_enough_to_attack = 1 - pow((1 - p_one_attacker_has_enough_to_attack), *Insurer::ATTACKS_PER_EPOCH);   // TODO shouldn't this be expected number of attacks, and not ATTACKS_PER_EPOCH?? 
    assert(p_at_least_one_attacker_has_enough_to_attack >= 0);
    assert(p_at_least_one_attacker_has_enough_to_attack <= 1);

    double p_loss = p_getting_attacked * p_at_least_one_attacker_has_enough_to_attack * attacking_expected_gains_outweigh_expected_costs * (1 - estimated_posture);
    assert(p_loss >= 0);
    assert(p_loss <= 1);

    PolicyType policy;
    policy.premium = (uint32_t) (p_loss * total_losses) / (retention_regression_factor * p_loss + loss_ratio);
    policy.retention = (uint32_t) retention_regression_factor * policy.premium;

    assert(policy.premium > 0); // I'd like to not have to consider cases where premium = 0
    assert(policy.retention > 0);

    return policy;
}


// Insurers use their overhead to conduct operations and perform risk analysis
// which informs current defender risks before writing policies.
void Insurer::perform_market_analysis(){
    
    // TODO TODO TODO should Insurers lose 20% of their assets each round as part of operating overhead?


    std::vector<double> attacker_assets;

    for (auto a = attackers->begin(); a != attackers->end(); ++a) {
        if (a->alive) {
            attacker_assets.push_back(a->assets);
        }
    }

    // Compute sample mean and variance
    double sampleMean = utils::computeMean(attacker_assets);
    double sampleVariance = utils::computeVariance(attacker_assets, sampleMean);

    // Compute parameters using method of moments
    double variance_mom = log(1 + pow(sampleVariance, 2) / pow(sampleMean, 2));
    double mu_mom = log(sampleMean) - 0.5 * variance_mom;
    double sigma_mom = sqrt(variance_mom);

    estimated_current_attacker_wealth_mean    = mu_mom;
    estimated_current_attacker_wealth_stdddev = sigma_mom;
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