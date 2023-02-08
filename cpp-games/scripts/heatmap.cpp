#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
using namespace std;



vector<string> splitline(string line) {
    vector<string> v;
 
    stringstream ss(line);
 
    while (ss.good()) {
        string substr;
        if (ss.peek() == '"') {
            getline(ss, substr, '"');
            getline(ss, substr, '"');
            v.push_back(substr);
            getline(ss, substr, ',');
        } else {
            getline(ss, substr, ',');
            v.push_back(substr);
        }  
    }

    return v;
}

int main() {
    ifstream infile("../logs/MANDATE=0.3.csv");
    string line;
    int count = 0;
    getline(infile, line); // throwaway first line
    while (getline(infile, line))
    {       


        count += 1; 
        // First parse the line
        vector<string> split = splitline(line);

        float MANDATE = stof(split[0]);
        float ATTACKERS = stof(split[1]);
        float INEQUALITY = stof(split[2]);
        float PREMIUM = stof(split[3]);
        float EFFICIENCY = stof(split[4]);
        float EFFORT = stof(split[5]);
        float PAYOFF = stof(split[6]);
        float CAUGHT = stof(split[7]);
        float CLAIMS = stof(split[8]);
        float TAX = stof(split[9]);
        int d_init = stoi(split[10]);
        int d_end = stoi(split[11]);
        int a_init = stoi(split[12]);
        int a_end = stoi(split[13]);
        int i_init = stoi(split[14]);
        int i_end = stoi(split[15]);
        int g_init = stoi(split[16]);
        int g_end = stoi(split[17]);
        int attacks_attempted = stoi(split[18]);
        int attacks_succeeded = stoi(split[19]);
        int amount_stolen = stoi(split[20]);
        int attacker_expenditures = stoi(split[21]);
        int government_expenditures = stoi(split[22]);
        string crossovers = split[23];
        string insurer_tod = split[24];
        int paid_claims = stoi(split[25]);
        int final_iter = stoi(split[26]);
        string outcome = split[27];

        // Turn into a metric of efficiency
        float initial_assets = d_init / MANDATE;
        float efficiency = (initial_assets - d_end) / initial_assets;

        // cout << efficiency << endl;

        // if (count % 1000000 == 0) {
        //     cout << count << endl;
        // }

        
        
    }
    return 0;
}