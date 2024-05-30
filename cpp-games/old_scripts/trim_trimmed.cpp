#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <math.h>
#include <stdio.h>
#include <algorithm>

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


// This program takes the sorted results in ../sorted/ and selects only
// the games that actually see action (i.e. attacks_attempted != 0)
int main(int argc, char *argv[]) {

    ofstream out[TAX_size][PREMIUM_size];

    // This is the results from trimming the trimmed_with_coMANDATEs values
    string logname = "trim_trimmed_log.txt";
    ofstream logfile;
    logfile.open(logname, std::ios_base::app); // append instead of overwrite

    logfile << "TAX,PREMIUM,";
    for (int k=0; k<MANDATE_size; k++) {
        logfile << "keep" << k << ",neighbors" << k << ",toss" << k;
        if (k == MANDATE_size - 1) {
            logfile << endl;
        } else {
            logfile << ",";
        }
    }
    

    for (int i=3; i<=3; i++) {
        for (int j=10; j<=10; j++) {
            

            string taxstr = i<10 ? "0." + to_string(i): "1.0";
            string premstr = j<10 ? "0." + to_string(j): "1.0";
            
            // int keep[MANDATE_size] = {0};
            // int keep_for_neighbors[MANDATE_size] = {0};
            // int toss[MANDATE_size] = {0};

            // ifstream infiles[MANDATE_size];
            // ofstream outfiles[MANDATE_size];


            for (int k=0; k<MANDATE_size; k++) {


                string mandstr = "0." + to_string(k);

                cout << "Loading MANDATE=" << mandstr << ", TAX=" << taxstr << ", PREMIUM=" << premstr << "..."<< flush;

                string ifilename = "../trimmed_with_coMANDATEs/MANDATE=" + mandstr + "_TAX=" + taxstr + "_PREMIUM=" + premstr + ".csv";
                string ofilename = "../trimmed/MANDATE=" + mandstr + "_TAX=" + taxstr + "_PREMIUM=" + premstr + ".csv";
                // infiles[k].open(ifilename);
                // outfiles[k].open(ofilename);
                // cout << "." << flush;

                ifstream infile;
                ofstream outfile;
                infile.open(ifilename);
                outfile.open(ofilename);

                string line;
                getline(infile, line);
                outfile << line << endl;

                while(getline(infile, line)) {
                    vector<string> split = splitline(line);
                    int amount_stolen = stoi(split[20]);
                     if (amount_stolen > 0 ) {
                         outfile << line << endl;
                     }
                }

                infile.close();
                outfile.close();

                cout << "done!" << endl;

            }
        }
    }
}