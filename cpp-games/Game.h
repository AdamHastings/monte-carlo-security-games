#pragma once

#include <sstream>
#include <vector>
#include <set>
#include "Player.h"

struct Params {
    double MANDATE;
    double ATTACKERS;
    double INEQUALITY;
    double PREMIUM;
    double EFFICIENCY;
    double EFFORT;
    double PAYOFF;
    double CAUGHT;
    double CLAIMS;
    double TAX;
    double DELAY;

    int B;
    int N;
    int E;
    int D;

    bool verbose;
    bool assertions_on;
    std::string logname;

    bool uniform;

    std::string to_string() {
        std::string str = "";
        str += "MANDATE="    + std::to_string(MANDATE)    + ",";
        str += "ATTACKERS="  + std::to_string(ATTACKERS)  + ",";
        str += "INEQUALITY=" + std::to_string(INEQUALITY) + ",";
        str += "PREMIUM="    + std::to_string(PREMIUM)    + ",";
        str += "EFFICIENCY=" + std::to_string(EFFICIENCY) + ",";
        str += "EFFORT="     + std::to_string(EFFORT)     + ",";
        str += "PAYOFF="     + std::to_string(PAYOFF)     + ",";
        str += "CAUGHT="     + std::to_string(CAUGHT)     + ",";
        str += "CLAIMS="     + std::to_string(CLAIMS)     + ",";
        str += "TAX="        + std::to_string(TAX)        + ",";
        str += "\n";
        return str;
    }
};

class Game {

    public:
        Game(Params &prm, std::vector<Defender> &d, std::vector<Attacker> &a, Insurer &i, Government &g);
        void run_iterations();

        std::string to_string();

        //  // Overload the "<<" operator for outputting MyClass objects
        // friend std::ostream& operator<<(std::ostream& os, const Game& obj)
        // {
        //     // Print the x and y values of the object
        //     os << "TODO implement << overload" << std::endl;
        //     return os;
        // }

    private:

        Params p;
        std::vector<Defender> defenders;
        std::vector<Attacker> attackers;
        Insurer insurer;
        Government government;

        double d_init, a_init, i_init, g_init;
        int iter_num;
        std::vector<int> crossovers;
        std::vector<int> insurerTimesOfDeath;

        double current_defender_sum_assets = 0;
        double current_attacker_sum_assets = 0;
        double defender_iter_sum = 0, attacker_iter_sum = 0;

        std::vector<double> defenders_cumulative_assets;
        std::vector<double> attackers_cumulative_assets;
        std::vector<double> insurer_cumulative_assets;
        std::vector<double> government_cumulative_assets;

        std::vector<double> last_delta_defenders_changes;
        std::vector<double> last_delta_attackers_changes;

        int outside_epsilon_count_defenders;
        int outside_epsilon_count_attackers;

        // std::set<int> alive_attackers;
        // std::set<int> alive_defenders;
        std::vector<int> alive_attackers_indices; //(alive_attackers.begin(), alive_attackers.end()); // TODO maybe optimize this later
        std::vector<int> alive_defenders_indices; //(alive_defenders.begin(), alive_defenders.end()); // TODO maybe optimize this later. Try just using a list instead of having to copy to a list each time..?
        
        

        double paidClaims = 0;
        int attacksAttempted = 0, attacksSucceeded = 0;
        int roundAttacks = 0;
        int consecutiveNoAttacks=0;

        std::string final_outcome = "X";
        int caught = 0;
        double attackerExpenditures = 0, attackerLoots = 0;
        double governmentExpenditures = 0;

        void fight(Attacker &a, Defender &d);
        void conclude_game(std::string outcome);
        bool is_equilibrium_reached();

        void verify_init();
        void verify_outcome();

        void a_steals_from_d(Attacker &a, Defender &d, double loot);
        void d_gain(Defender &d, double gain);
        void d_lose(Defender &d, double loss);
        void a_gain(Attacker &a, double gain);
        void a_lose(Attacker &a, double loss);
        void d_recoup(Attacker &a, Defender &d, double recoup_amount);
        void insurer_lose(double loss);
        void insurer_covers_d_for_losses_from_a(Attacker &a, Defender &d, double claim);
        void insurer_recoup(double recoup);
        void government_gain(double gain);
        void government_lose(double loss);
        void a_distributes_loot(Attacker &a);

        // void buy_insurance(Defender &d);

};