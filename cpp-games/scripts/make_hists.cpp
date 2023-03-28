#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <math.h>
#include <stdio.h>
#include <algorithm>
#include <map>

using namespace std;

#define MANDATE_size  6
#define TAX_size 11
#define PREMIUM_size 11

#define NUM_PARAMS 10
#define PARAM_RANGE 11 

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

int main(int argc, char *argv[]) {

    // initialize holding container
    map<string, vector<int>> hist_vals;
    string params[NUM_PARAMS] = {"MANDATE","ATTACKERS","INEQUALITY","PREMIUM","EFFICIENCY","EFFORT","PAYOFF","CAUGHT","CLAIMS","TAX"};
    for (string p : params) {

        hist_vals[p] = vector<int>(PARAM_RANGE, 0);
    }

    for (int i=0; i<TAX_size; i++) {
        for (int j=0; j<PREMIUM_size; j++) {
            for (int k=0; k<MANDATE_size; k++) {
                string taxstr = i<10 ? "0." + to_string(i): "1.0";
                string premstr = j<10 ? "0." + to_string(j): "1.0";
                string mandstr = "0." + to_string(k);

                string ifilename = "../trimmed/MANDATE=" + mandstr + "_TAX=" + taxstr + "_PREMIUM=" + premstr + ".csv";
                cout << "loading " << ifilename << "..." << flush;
                ifstream infile;
                infile.open(ifilename);

                string line;

                // throwaway first header line
                getline(infile, line);

                while(getline(infile, line)) {
                    vector<string> split = splitline(line);
                    if (split.size() != 28) {
                        cout << "split.size() != 28!" << endl;
                    }

                    int MANDATE = (int) round(stod(split[0]) * 10);
                    int ATTACKERS = (int) round(stod(split[1]) * 10);
                    int INEQUALITY = (int) round(stod(split[2]) * 10);
                    int PREMIUM = (int) round(stod(split[3]) * 10);
                    int EFFICIENCY = (int) round(stod(split[4]) * 10);
                    int EFFORT = (int) round(stod(split[5]) * 10);
                    int PAYOFF = (int) round(stod(split[6]) * 10);
                    int CAUGHT = (int) round(stod(split[7]) * 10);
                    int CLAIMS = (int) round(stod(split[8]) * 10);
                    int TAX = (int) round(stod(split[9]) * 10);


                    hist_vals["MANDATE"][MANDATE]++;
                    hist_vals["ATTACKERS"][ATTACKERS]++;
                    hist_vals["INEQUALITY"][INEQUALITY]++;
                    hist_vals["PREMIUM"][PREMIUM]++;
                    hist_vals["EFFICIENCY"][EFFICIENCY]++;
                    hist_vals["EFFORT"][EFFORT]++;
                    hist_vals["PAYOFF"][PAYOFF]++;
                    hist_vals["CAUGHT"][CAUGHT]++;
                    hist_vals["CLAIMS"][CLAIMS]++;
                    hist_vals["TAX"][TAX]++;
                }

                ofstream outfile;
                outfile.open("make_hists_log.txt");

                cout << "writing log..." << flush;
                outfile << "param,0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0" << endl;
                for (auto p : hist_vals) {
                    string param = p.first;
                    vector<int> counts = p.second;

                    outfile << param << ",";

                    for (int c=0; c<PARAM_RANGE; c++) {
                        if (c == 0 && (param == "ATTACKERS" || param == "INEQUALITY" || param == "EFFICIENCY" || param == "EFFORT" || param == "PAYOFF" || param == "CLAIMS")) {
                            outfile << ",";
                        }
                        else if (c > 5 && param == "MANDATE") {
                            outfile << "";
                            if (c == PARAM_RANGE - 1) {
                                outfile << endl;
                            } else {
                                outfile << ",";
                            }

                        } else {
                            outfile << counts[c];
                            if (c == PARAM_RANGE - 1) {
                                outfile << endl;
                            } else {
                                outfile << ",";
                            }
                        }
                    }
                }
                cout << "done!" << endl;
            }
        }
    }
}