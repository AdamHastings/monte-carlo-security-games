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


int main(int argc, char *argv[]) {



    ifstream infiles[MANDATE_size];


    long long best_ctr[MANDATE_size][TAX_size][PREMIUM_size] = {0};

    for (int i=0; i<TAX_size; i++) {
        for (int j=0; j<PREMIUM_size; j++) {
            string taxstr = i<10 ? "0." + to_string(i): "1.0";
            string premstr = j<10 ? "0." + to_string(j): "1.0";

            cout << "Loading --- TAX=" << taxstr << ", PREMIUM=" << premstr << flush;

            for (int k=0; k<MANDATE_size; k++) {
                string mandstr = "0." + to_string(k);
                string ifilename = "../sorted/MANDATE=" + mandstr + "_TAX=" + taxstr + "_PREMIUM=" + premstr + ".csv";
                infiles[k].open(ifilename);
                cout << "." << flush;
            }

            cout << " loaded..." << flush;
            // vector<vector<string>> linesv;
            double loss_pcts[MANDATE_size];

            // throw away the first line
            for (int k=0; k<MANDATE_size; k++) {
                string line;
                getline(infiles[k], line);
            }

            int lc_max = 11000000;
            for (int lc = 0; lc < lc_max; lc ++) {
                for (int k=0; k<MANDATE_size; k++) {
                    string line;
                    getline(infiles[k], line);   
                    // First parse the line
                    vector<string> split = splitline(line);

                    // linesv[k] = split;

                    string MANDATE = split[0];
                    string ATTACKERS = split[1];
                    string INEQUALITY = split[2];
                    string PREMIUM = split[3];
                    string EFFICIENCY = split[4];
                    string EFFORT = split[5];
                    string PAYOFF = split[6];
                    string CAUGHT = split[7];
                    string CLAIMS = split[8];
                    string TAX = split[9];
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

                    int taxidx = round(stod(TAX) * 10);
                    int premidx = round(stod(PREMIUM) * 10);

                    loss_pcts[k] = d_end/d_init;
                }

                double max = 0;
                int max_k = 0;
                for (int k=0; k<MANDATE_size; k++) {
                    if (loss_pcts[k] > max) {
                        max = loss_pcts[k];
                        max_k = k;
                    }
                }

                best_ctr[max_k][i][j] += 1;
            }

            for (int k=0; k<MANDATE_size; k++) {
                infiles[k].close();
            }

            cout << "done!" << endl;
        }

        ofstream parse;
        parse.open("parsed.csv");
        for (int ii=0; ii<TAX_size; ii++) {
            for (int jj=0; jj<PREMIUM_size; jj++) {
                for (int kk=0; kk<MANDATE_size; kk++) {
                    parse << best_ctr[kk][ii][jj] << ",";
                }
                parse << endl;
            }
        }
        parse.close();
    }

    return 0;
}