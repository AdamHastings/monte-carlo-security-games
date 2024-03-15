#include "Player.h"
#include <random>
#include <map>
#include <iostream>
#include <algorithm>
#include <vector>



Player::Player(Params &p_in) {
    assets = 0;
    p = p_in;
}

void Player::gain(double gain) {
    assets += gain;
}

void Player::lose(double loss) {
    assets -= loss;
    if (assets == 0){
        alive = false;
    }
}

double Player::get_assets() {
    return assets;
}


uint Insurer::i_init = 0; // Initialization outside the class definition


Insurer::Insurer(int id_in, Params &p) : Player(p) {
    id = id_in;

    assets = p.WEALTH_distribution->draw();
    if (assets < 0) {
        assets = 0;
    }

    i_init += assets; 
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


// Should be set each iteration by the game. 
int Insurer::num_defenders = 0;
int Insurer::num_attackers = 0;
std::vector<double> Insurer::attacker_assets;

double Insurer::insurer_iter_sum = 0;


PolicyType Insurer::provide_a_quote(double assets, double estimated_posture, double estimated_costToAttackPercentile) {    
    PolicyType policy;

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

Defender::Defender(int id_in, Params &p, std::vector<Insurer>& _insurers) : Player(p) {
    id = id_in;

    insurers = &_insurers;

    assets = p.WEALTH_distribution->draw();
    if (assets < 0) {
        assets = 0;
    }

    posture = p.POSTURE_distribution->draw();
    if (posture < 0) {
        posture = 0;
    } else if (posture > 1) {
        posture = 1;
    }

    costToAttack = assets * posture;

    d_init += assets;
}

double Defender::estimated_probability_of_attack = 0;

uint Defender::d_init = 0; 

double Defender::defender_iter_sum = 0;



// TODO this is why the checksums are failing...transactions not known to Game class
void Defender::purchase_insurance_policy(Insurer &i, PolicyType p) {
    insured = true;
    ins_idx = i.id;
    lose(p.premium);
    i.gain(p.premium);
}

void Defender::submit_claim(double loss) {
    assert(insured); // you should only call this function if you have an active insurance policy
    assert(ins_idx >= 0); 
    double amount_recovered = insurers->at(ins_idx).issue_payment(loss);
    assert(loss >= amount_recovered);
    lose(loss - amount_recovered);
}

// TODO this is why the checksums are failing...transactions not known to Game class
void Defender::make_security_investment(double x) {
    double sec_investment_efficiency_draw = p.EFFICIENCY_distribution->draw();
    posture = std::min(1.0, posture*(1 + sec_investment_efficiency_draw * (x / (assets*1.0))));
    assert(posture >= 0);
    assert(posture <= 1);
    lose(x);
    costToAttack = assets * posture; // Why wasn't this included until now?
    assert(costToAttack >= 0);
}

// TODO this is only for one insurer...shouldn't Defender query all Insurers?
void Defender::choose_security_strategy() {

    assert(insurers->size() >= 0);

    Insurer i = insurers->at(0); // TODO iterate through all insurers!!!
    
    double p_A_hat = estimated_probability_of_attack;
    double p_L_hat = p_A_hat * (1 - posture);
    double mean_EFFICIENCY = p.EFFICIENCY_distribution->mean();
    double mean_PAYOFF = p.PAYOFF_distribution->mean();

    // 1. Get insurance policy from insurer
    std::cout << "costToAttackPercentile: " << costToAttackPercentile << std::endl; // TODO why is this nan?
    PolicyType policy = i.provide_a_quote(assets, posture, costToAttackPercentile); // TODO add noise to posture? or costToAttackPercentile?
    std::cout << policy.premium << " " << policy.retention << " " << p_L_hat << std::endl;
    double expected_loss_with_insurance = policy.premium +(p_L_hat * policy.retention);
    std::cout << "expected_loss_with_insurance: " << expected_loss_with_insurance << std::endl;
    assert(expected_loss_with_insurance >= 0);

    // 2. Find optimum security investment
    double optimal_investment = std::min(assets, std::max(0.0, (assets * (-1 + (assets * (mean_PAYOFF + posture * (-1 + mean_EFFICIENCY) * mean_PAYOFF))))/(2 * posture * p_A_hat * mean_EFFICIENCY * mean_PAYOFF)));
    double expected_loss_with_optimal_investment = std::max(0.0, (assets - optimal_investment) * mean_PAYOFF * (p_A_hat * (1 - (posture * (mean_EFFICIENCY * (optimal_investment/assets))))) + optimal_investment);
    assert(optimal_investment >= 0);
    assert(expected_loss_with_optimal_investment >= 0);


    // 3. Find cost to achieve perfect security
    double perfect_security_investment = (assets * (assets * posture)) / (posture * mean_EFFICIENCY);
    double expected_loss_with_perfect_security = perfect_security_investment;
    assert(perfect_security_investment >= 0);
    assert(expected_loss_with_perfect_security >= 0);

    // Choose the optimal strategy.
    double minimum = std::min({expected_loss_with_insurance, expected_loss_with_optimal_investment,expected_loss_with_perfect_security});
    if (minimum == expected_loss_with_insurance) {
        purchase_insurance_policy(i, policy);
    } else if (minimum == expected_loss_with_optimal_investment) {
        make_security_investment(optimal_investment);
    } else if (minimum == expected_loss_with_perfect_security) {
        make_security_investment(perfect_security_investment);
    } else {
        assert(false); // should never reach this
    }
}

uint Attacker::a_init = 0; 
double Attacker::attacker_iter_sum = 0;

Attacker::Attacker(int id_in, Params &p) : Player(p) {
    id = id_in;

    assets = p.WEALTH_distribution->draw() * p.INEQUALITY; // TODO check this...this doesn't need to a distribution, right?
    if (assets < 0) {
        assets = 0;
    }

    a_init += assets; // TODO should this be a static class variable?
}

void Insurer::lose(double loss) {
    Player::lose(loss);
    insurer_iter_sum += loss;
}

void Insurer::gain(double gain) {
    Player::gain(gain);
    insurer_iter_sum += gain;
}


void Defender::lose(double loss) {
    Player::lose(loss);
    defender_iter_sum += loss;
}

void Defender::gain(double gain) {
    Player::gain(gain);
    defender_iter_sum += gain;
}


void Attacker::lose(double loss) {
    Player::lose(loss);
    attacker_iter_sum += loss;
}

void Attacker::gain(double gain) {
    Player::gain(gain);
    attacker_iter_sum += gain;
}
