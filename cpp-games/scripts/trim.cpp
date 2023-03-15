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

    string logname = "trimmed_log.txt";
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
    

    for (int i=0; i<TAX_size; i++) {
        for (int j=0; j<PREMIUM_size; j++) {
            

            string taxstr = i<10 ? "0." + to_string(i): "1.0";
            string premstr = j<10 ? "0." + to_string(j): "1.0";
            
            int keep[MANDATE_size] = {0};
            int keep_for_neighbors[MANDATE_size] = {0};
            int toss[MANDATE_size] = {0};

            ifstream infiles[MANDATE_size];
            ofstream outfiles[MANDATE_size];

            cout << "Loading TAX=" << taxstr << ", PREMIUM=" << premstr << "..."<< flush;

            for (int k=0; k<MANDATE_size; k++) {
                string mandstr = "0." + to_string(k);
                string ifilename = "../sorted/MANDATE=" + mandstr + "_TAX=" + taxstr + "_PREMIUM=" + premstr + ".csv";
                string ofilename = "../trimmed/MANDATE=" + mandstr + "_TAX=" + taxstr + "_PREMIUM=" + premstr + ".csv";
                infiles[k].open(ifilename);
                outfiles[k].open(ofilename);
                cout << "." << flush;
            }

            cout << "loaded! Parsing..." << flush;

            string lines[MANDATE_size];

            // handle the first line
            for (int k=0; k<MANDATE_size; k++) {
                getline(infiles[k], lines[k]);
                outfiles[k] << lines[k] << endl;
            }

            int amount_stolen[MANDATE_size] = {0};

            int lc_max = 11000000;
            for (int lc = 0; lc < lc_max; lc ++) {
                for (int k=0; k<MANDATE_size; k++) {
                    getline(infiles[k], lines[k]);   
                    vector<string> split = splitline(lines[k]);
                    amount_stolen[k] = stoi(split[20]);
                }

                int* max = max_element(amount_stolen, amount_stolen + MANDATE_size);
                
                if (*max > 0) {
                    for (int k=0; k<MANDATE_size; k++) {
                        outfiles[k] << lines[k] << endl;
                        if (amount_stolen[k] == 0 ) {
                            keep_for_neighbors[k]++;
                        } else {
                            keep[k]++;
                        }
                    }  
                } else {
                    for (int k=0; k<MANDATE_size; k++) {
                        toss[k]++;
                    }
                }
            }

            // write logfile
            logfile << taxstr << "," << premstr << ",";
            for (int k=0; k<MANDATE_size; k++) {
                logfile << keep[k] << "," << keep_for_neighbors[k] << ",";
                if (k == MANDATE_size - 1) {
                    logfile << toss[k] << endl;
                } else {
                    logfile << toss[k] << ",";
                }
            }


            // for (int k=0; k<MADATE_size; k++) {
            //     string mandstr =  "0." + to_string(k);

            //     string filename_stub = "MANDATE=" + mandstr + "_TAX=" + taxstr + "_PREMIUM=" + premstr + ".csv";
            //     string ifilename = "../sorted/" + filename_stub;
            //     string ofilename = "../trimmed/" filename_stub;

            //     ifstream infile;
            //     ofstream outfile;
            //     infile.open(ifilename);
            //     outfile.open(ofilename);

            //     string line;
            //     getline(infile, line); // handle first line header
            //     ofstream << line << endl;

            //     // iterate lines
            //     while (getline(infile, line)) {
            //         vector<string> split = splitline(line);
            //         int attacks_attempted = stoi(split[18]);

            //         if (attacks_attempted > 0) {
            //             ofstream << line << endl;
            //             keep++;
            //         } else {
            //             toss++;
            //         }
            //     }           
            // }

            for (int k=0; k<MANDATE_size; k++) {
                infiles[k].close();
                outfiles[k].close();
            }

            cout << "done!" << endl;


        }
    }
}