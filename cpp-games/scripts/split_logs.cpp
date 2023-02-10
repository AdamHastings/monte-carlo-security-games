#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <math.h>
#include <stdio.h>

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

    string filename = "../logs/MANDATE=0.0.csv";
    ifstream infile(filename);
    string line;
    int count = 0;
    int splitcount = 0;
    getline(infile, line); // throwaway first line

    ofstream outfile;
    string outfilename = "../plausible/plausible_MANDATE=0.0.csv";
    if( remove(outfilename.c_str()) != 0 ) {
        cout << "Error deleting file" << endl;
    } else {
        cout << "File successfully deleted" << endl;
    }

    outfile.open(outfilename, std::ios_base::app); // append instead of overwrite
    outfile << line << endl;

    while (getline(infile, line)) {       
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

        if (amount_stolen > 0) {
            if (TAX == 0 && PREMIUM == 0) {
                outfile << line << endl;
                splitcount += 1;
            }
        }
    }

    outfile.close();

    return 0;
}