#include <algorithm>
#include <numeric>
#include <cmath>
#include "Insurer.h"
#include "Defender.h"
#include "Attacker.h"
#include "utils.h"

// Initialization outside the class definition
unsigned long long Insurer::i_init = 0; 
int64_t Insurer::current_sum_assets = 0;
int64_t Insurer::insurer_iter_sum = 0;
int64_t Insurer::sum_premiums_collected = 0;
unsigned long long Insurer::operating_expenses = 0;
unsigned long long Insurer::paid_claims = 0;

double Insurer::estimated_current_attacker_wealth_mean = 0;
double Insurer::estimated_current_attacker_wealth_stdddev = 0;
double Insurer::loss_ratio = 0;
double Insurer::retention_regression_factor = 0;

double Insurer::p_attack = 0;

// Game Insurer::g;

unsigned int* Insurer::ATTACKS_PER_EPOCH;

std::vector<unsigned long long> Insurer::cumulative_assets; 

std::vector<Attacker>* Insurer::attackers;


Insurer::Insurer(int id_in, Params &p) : Player(p) {
    id = id_in;
    assert(id >= 0);

    double fp_assets = p.WEALTH_distribution->draw() * pow(10, 6); // In terms of thousands. Baseline params in terms of billions. 
    assert(fp_assets < __UINT32_MAX__);
    assets = (uint32_t) fp_assets;

    i_init += assets; 
    current_sum_assets += assets;
}

void Insurer::lose(int64_t loss) {
    Player::lose(loss);
    this->round_losses -= loss;
    insurer_iter_sum -= loss;
    current_sum_assets -= loss;
}

void Insurer::gain(int64_t gain) {
    Player::gain(gain);
    insurer_iter_sum += gain;
    current_sum_assets += gain;
}

int64_t Insurer::issue_payment(int64_t claim) {
    
    int64_t amount_covered;
    if (claim > assets) { // insurer cannot cover full amount and goes bust
        amount_covered = assets;
    } else {
        amount_covered = claim;
    }
    this->lose(amount_covered); 
    paid_claims += amount_covered; 
    return amount_covered;
}

void Insurer::sell_policy(PolicyType policy) {
    Insurer::sum_premiums_collected += policy.premium;
    this->gain(policy.premium); 
}

PolicyType Insurer::provide_a_quote(int64_t assets, double estimated_posture) { 

    long long ransom = Defender::ransom_cost(assets);
    long long recovery_cost = Defender::recovery_cost(assets);
    long long total_losses = ransom + recovery_cost;
    
    int64_t expected_cost_to_attack = (int64_t) (p.CTA_SCALING_FACTOR_distribution->mean() * Attacker::estimated_current_defender_posture_mean * ransom); 

    double p_one_attacker_has_enough_to_attack;
    if (std::isnan(estimated_current_attacker_wealth_stdddev) || estimated_current_attacker_wealth_stdddev == 0) { // There is only one attacker, so using CDF doesn't make sense 
        p_one_attacker_has_enough_to_attack = (exp(estimated_current_attacker_wealth_mean) > expected_cost_to_attack) ? true : false;
    } else {
        p_one_attacker_has_enough_to_attack =  1 -  0.5 * (1 + erf((log(expected_cost_to_attack) - estimated_current_attacker_wealth_mean) / (estimated_current_attacker_wealth_stdddev * sqrt(2)))); // CDF of lognormal distribution
    }
    assert(p_one_attacker_has_enough_to_attack >= 0);
    assert(p_one_attacker_has_enough_to_attack <= 1);

    double p_at_least_one_attacker_has_enough_to_attack = 1 - pow((1 - p_one_attacker_has_enough_to_attack), *Insurer::ATTACKS_PER_EPOCH);   // TODO shouldn't this be expected number of attacks, and not ATTACKS_PER_EPOCH?? 
    assert(p_at_least_one_attacker_has_enough_to_attack >= 0);
    assert(p_at_least_one_attacker_has_enough_to_attack <= 1);

    double p_loss = p_attack * p_at_least_one_attacker_has_enough_to_attack * (1 - estimated_posture);
    assert(p_loss >= 0);
    assert(p_loss <= 1);

    PolicyType policy;
    policy.premium = (int64_t) (p_loss * total_losses) / (retention_regression_factor * p_loss + loss_ratio);
    policy.retention = (int64_t) retention_regression_factor * policy.premium;

    if (policy.premium != 0) {
        assert(policy.premium > 0);
        assert(policy.retention > 0);
    }

    return policy;
}


// Insurers use their overhead to conduct operations and perform risk analysis
// which informs current defender risks before writing policies.
void Insurer::perform_market_analysis(std::vector<Insurer> &insurers, int current_num_defenders){
    
    // Insurers spend previous round's earnings on operating expenses 
    for (uint j=0; j < insurers.size(); j++) {
        Insurer *i = &insurers[j];

        if (i->is_alive()) {            
            // must be in terms of losses, not earnings, because earnings are based on expected losses
            // whereas losses are based on real losses!
            // i.e. don't do this: // unsigned int allowed_spending = (int) (((double) i->round_losses / loss_ratio) -  i->round_losses);
            
            int64_t last_round_losses = i->round_losses;
            i->round_losses = 0;

            int64_t allowed_spending = (int64_t) ((abs((double) last_round_losses)) / loss_ratio - abs(last_round_losses));

            if (allowed_spending > i->assets) {
                allowed_spending = i->assets;
            }
            i->lose(allowed_spending);
            operating_expenses += allowed_spending;  
        }
    }

    std::vector<double> attacker_assets;

    // This can be optimized by passing in alive_attackers_indices instead
    for (auto a = attackers->begin(); a != attackers->end(); ++a) {
        if (a->is_alive()) {
            attacker_assets.push_back(a->assets);
        }
    }
    assert(attacker_assets.size() > 0);

    // Compute parameters using method of moments
    // https://web.archive.org/web/20180423000433id_/https://scholarsarchive.byu.edu/cgi/viewcontent.cgi?article=2927&context=etd
    // TODO: cite this!
    estimated_current_attacker_wealth_mean    = utils::compute_mu_mom(attacker_assets); // Note!! This is the log of actual lognormal mean!!
    assert(estimated_current_attacker_wealth_mean >= 0);

    // Can return nan. Need to check against this condition elsewhere.
    estimated_current_attacker_wealth_stdddev = sqrt(utils::compute_var_mom(attacker_assets)); // Note!! This is the log of actual lognormal stddev!!
    assert(std::isnan(estimated_current_attacker_wealth_stdddev) ? attacker_assets.size() == 1 : true);
    assert(estimated_current_attacker_wealth_stdddev == 0 ? attacker_assets.size() == 1 : true);

    
    // Makes the assumption that defenders will only be attacked once per epoch
    // A reasonable assumption first of all
    // And second, it makes the math orders of magnitude easier 
    double p_getting_paired_with_attacker_a = std::min(1.0, (*Insurer::ATTACKS_PER_EPOCH * 1.0) / (current_num_defenders * 1.0));
    assert(p_getting_paired_with_attacker_a >= 0);
    assert(p_getting_paired_with_attacker_a <= 1);

    double p_getting_attacked;
    if (p_getting_paired_with_attacker_a == 1.0) {
        p_getting_attacked = 1.0;
    } else {
       p_getting_attacked = 1 - pow((1 - p_getting_paired_with_attacker_a), (double) attacker_assets.size());
    } 
    assert(p_getting_attacked >= 0);
    assert(p_getting_attacked <= 1);

    double expected_cta_scaling_factor = p.CTA_SCALING_FACTOR_distribution->mean();
    bool attacking_expected_gains_outweigh_expected_costs = (Attacker::estimated_current_defender_posture_mean < (1.0/(1 + expected_cta_scaling_factor)));

    p_attack = p_getting_attacked * attacking_expected_gains_outweigh_expected_costs;
}

void Insurer::reset(){
    i_init = 0;
    insurer_iter_sum = 0;
    current_sum_assets = 0;
    sum_premiums_collected = 0;
    operating_expenses = 0;
    paid_claims = 0;
    estimated_current_attacker_wealth_mean = 0;
    estimated_current_attacker_wealth_stdddev = 0;
    loss_ratio = 0;
    retention_regression_factor = 0;
    attackers = nullptr;
    ATTACKS_PER_EPOCH = nullptr;
    operating_expenses = 0;
    cumulative_assets.clear();
    p_attack = 0;
}