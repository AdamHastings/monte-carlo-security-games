/*
*
* This is meant to be a ONE-PASS program that compute all necessary summary statistics from the ../trimmed/ data
* I.e., don't re-parse through all of ../trimmed/ just to compute one set of summary statistics. Do it all here. It will keep the repository much cleaner.
*
*/



#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <math.h>
#include <stdio.h>

using namespace std;

#define MANDATE_size  6
#define TAX_size 11
#define PREMIUM_size 11

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

struct Line {
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
    int d_init;
    int d_end;
    int a_init;
    int a_end;
    int i_init;
    int i_end;
    int g_init;
    int g_end;
    int attacks_attempted;
    int attacks_succeeded;
    int amount_stolen;
    int attacker_expenditures;
    int government_expenditures;
    string crossovers;
    string insurer_tod;
    int paid_claims;
    int final_iter;
    string outcome;
};

Line linestr_to_line(string linestr) {
    Line line;
    vector<string> split = splitline(linestr);

    line.MANDATE = stod(split[0]);
    line.ATTACKERS = stod(split[1]);
    line.INEQUALITY = stod(split[2]);
    line.PREMIUM = stod(split[3]);
    line.EFFICIENCY = stod(split[4]);
    line.EFFORT = stod(split[5]);
    line.PAYOFF = stod(split[6]);
    line.CAUGHT = stod(split[7]);
    line.CLAIMS = stod(split[8]);
    line.TAX = stod(split[9]);
    line.d_init = stoi(split[10]);
    line.d_end = stoi(split[11]);
    line.a_init = stoi(split[12]);
    line.a_end = stoi(split[13]);
    line.i_init = stoi(split[14]);
    line.i_end = stoi(split[15]);
    line.g_init = stoi(split[16]);
    line.g_end = stoi(split[17]);
    line.attacks_attempted = stoi(split[18]);
    line.attacks_succeeded = stoi(split[19]);
    line.amount_stolen = stoi(split[20]);
    line.attacker_expenditures = stoi(split[21]);
    line.government_expenditures = stoi(split[22]);
    line.crossovers = split[23];
    line.insurer_tod = split[24];
    line.paid_claims = stoi(split[25]);
    line.final_iter = stoi(split[26]);
    line.outcome = split[27];

    return line;
}

void insurance_init() {
    // max i_init = 115057498
    // Bounds will be 0 - 150000000 at 10000000 increments
}

void insurance_step() {

}

void insurance_end() {

}

int main(int argc, char *argv[]) {


    // For each "pass", just call an init here, then do your processing in loop, and then a post-loop step
    insurance_init();

    // Iterate through all the files
    for (int i=0; i<TAX_size; i++) {
        for (int j=0; j<PREMIUM_size; j++) {
            for (int k=0; k<MANDATE_size; k++) {

                string taxstr = i<10 ? "0." + to_string(i): "1.0";
                string premstr = j<10 ? "0." + to_string(j): "1.0";
                string mandstr = "0." + to_string(k);
                
                string ifilename = "../trimmed/MANDATE=" + mandstr + "_TAX=" + taxstr + "_PREMIUM=" + premstr + ".csv";
                ifstream infile(ifilename);

                string linestr;
                getline(infile, linestr); // throwaway first line

                while (getline(infile, linestr)) {    
                    Line line = linestr_to_line(linestr);
                    
                    // Do you in-loop steps here
                    insurance_step();
                }
            }
        }
    }

    // Do your post loop processing here
    insurance_end();


    return 0;
}