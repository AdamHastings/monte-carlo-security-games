#pragma once

#include "params.h"
#include "Player.h"
#include "Attacker.h"
#include "Defender.h"



class Insurer : public Player {
    public:
        uint id;
        static uint i_init;
        static double insurer_iter_sum;
        static std::vector<double> cumulative_assets; // running total of all insurers' assets
        static double current_sum_assets; // sum total of all class instances

        static double paid_claims;

        // TODO shouldn't this be static?
        static std::vector<Defender>* defenders;
        static std::vector<Attacker>* attackers;

        Insurer(int id_in, Params &p, std::vector<Defender>& _defenders, std::vector<Attacker>& _attackers);
        void gain(double gain) override;
        void lose(double loss) override;

        PolicyType provide_a_quote(double assets, double posture, double estimated_costToAttackPercentile);
        double issue_payment(double claim);
        static void perform_market_analysis(int prevRoundAttacks);

    private:
        static double findPercentile(const std::vector<double>& sortedVector, double newValue);

};