#pragma once

#include <sstream>
#include <vector>
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
        std::vector<int> crossovers;
        std::vector<int> insurerTimesOfDeath;

        float paidClaims = 0;
        int attacksAttempted = 0, attacksSucceeded = 0;
        char outcome = 0;
        int caught = 0;
        float attackerExpenditures = 0, attackerLoots = 0;


};