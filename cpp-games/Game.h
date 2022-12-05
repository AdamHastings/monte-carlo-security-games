#pragma once

#include <sstream>
#include <vector>
#include <set>
#include "Player.h"

struct Params {
    float MANDATE;
    float ATTACKERS;
    float INEQUALITY;
    float PREMIUM;
    float EFFICIENCY;
    float EFFORT;
    float PAYOFF;
    float CAUGHT;
    float CLAIMS;
    float TAX;

    int B;
    int N;
    int E;
    int D;

    bool verbose;

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
        Game(Params p, std::vector<Defender> d, std::vector<Attacker> a, Insurer i, Government g);
        void run_iterations();

         // Overload the "<<" operator for outputting MyClass objects
        friend std::ostream& operator<<(std::ostream& os, const Game& obj)
        {
            // Print the x and y values of the object
            os << "TODO implement << overload" << std::endl;
            return os;
        }

    private:

        Params p;
        std::vector<Defender> defenders;
        std::vector<Attacker> attackers;
        Insurer insurer;
        Government government;

        float d_init, a_init, i_init, g_init;
        int iter_num;
        std::vector<int> crossovers;
        std::vector<int> insurerTimesOfDeath;

        float current_defender_sum_assets = 0;
        float current_attacker_sum_assets = 0;
        float defender_iter_sum = 0, attacker_iter_sum = 0;

        std::vector<float> defenders_cumulative_assets;
        std::vector<float> attackers_cumulative_assets;
        std::vector<float> insurer_cumulative_assets;
        std::vector<float> government_cumulative_assets;

        std::vector<float> last_delta_defenders_changes;
        std::vector<float> last_delta_attackers_changes;

        int outside_epsilon_count_defenders = 0;
        int outside_epsilon_count_attackers = 0;

        std::set<int> alive_attackers;
        std::set<int> alive_defenders;
        

        float paidClaims = 0;
        int attacksAttempted = 0, attacksSucceeded = 0;
        std::string final_outcome = "X";
        int caught = 0;
        float attackerExpenditures = 0, attackerLoots = 0;

        void fight(Attacker a, Defender d);
        void conclude_game(std::string outcome);
        bool is_equilibrium_reached();
        void verify_state();

        void a_steals_from_d(Attacker a, Defender d, float loot);
        void d_gain(Defender d, float gain);
        void d_lose(Defender d, float loss);
        void a_gain(Attacker a, float gain);
        void a_lose(Attacker a, float loss);
        void d_recoup(Attacker a, Defender d, float recoup_amount);
        void insurer_lose(float loss);
        void insurer_covers_d_for_losses_from_a(Attacker a, Defender d, float claim);
        void insurer_recoup(float recoup);
        void government_gain(float gain);
        void government_lose(float loss);
        void a_distributes_loot(Attacker a);

};