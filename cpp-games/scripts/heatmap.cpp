#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <math.h>
using namespace std;

#define TAX_len 11
#define PREMIUM_len 11



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


double averages[TAX_len][PREMIUM_len] = {0};
int lc = 1331000000;
int fudge = 10000;
double eta = 1/lc * fudge;

vector<string> MANDATE_range = {"0.0", "0.1", "0.2", "0.3", "0.4"};

int main() {
    
    for (string s : MANDATE_range) {
    
        string filename = "../logs/MANDATE=" + s + ".csv";
        ifstream infile(filename);
        string line;
        int count = 0;
        getline(infile, line); // throwaway first line


        while (getline(infile, line))
        {       


            count += 1; 
            // First parse the line
            vector<string> split = splitline(line);

            double MANDATE = stod(split[0]);
            double ATTACKERS = stod(split[1]);
            double INEQUALITY = stod(split[2]);
            double PREMIUM = stod(split[3]);
            double EFFICIENCY = stod(split[4]);
            double EFFORT = stod(split[5]);
            double PAYOFF = stod(split[6]);
            double CAUGHT = stod(split[7]);
            double CLAIMS = stod(split[8]);
            double TAX = stod(split[9]);
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
            double initial_assets = d_init / MANDATE;
            double efficiency = (initial_assets - d_end) / initial_assets;

            int i = (int)round(TAX*10);
            int j = (int)round(PREMIUM*10);
            double result = (eta * (efficiency - averages[i][j]));
            averages[i][j] += result;

            // TODO remove later
            if (count >= 100000) {
                break;
            }
        }

        string outfilename = "averages_MANDATE=" + s + ".csv";
        ofstream outfile;
        outfile.open(outfilename);
        for (int i=0; i<TAX_len; i++) {
            for (int j=0; j<PREMIUM_len; j++) {
                outfile << averages[i][j] << ",";
            }
            outfile << "\n";
        }
        outfile.close();
    }


    // write to file
    return 0;
}