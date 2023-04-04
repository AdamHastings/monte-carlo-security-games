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
#include <cassert>
#include <math.h>
#include <stdio.h>

using namespace std;

#define MANDATE_size  6
#define TAX_size 11
#define PREMIUM_size 11
#define CLAIMS_size 10
#define CAUGHT_size 11
#define NUM_BUCKETS 1000 
#define ASSUMED_MAX_I_INIT 1000000000
#define I_INIT_DIVISOR (ASSUMED_MAX_I_INIT / NUM_BUCKETS) 

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

struct InsuranceHistItems {
    int midpoint = 0;
    int lowval = 0;
    int highval = 0;
    int count = 0;
    double avg_loss = 0;
    double avg_duration = 0;

    void update_avg_loss(double new_loss) {
        avg_loss = avg_loss + ((new_loss - avg_loss) / (double) count);
    }

    void update_avg_duration(int new_duration) {
        avg_duration = avg_duration + ((new_duration - avg_duration) / (double) count);
    }

    string to_string() {
        string ret = "";
        // TODO add details here
        return ret;
    }
};


InsuranceHistItems insurance_CLAIMS[NUM_BUCKETS][CLAIMS_size];
InsuranceHistItems insurance_CAUGHT[NUM_BUCKETS][CAUGHT_size];

void insurance_end() {

    // Write results to log file
    ofstream claims_outfile;
    claims_outfile.open("../scripts/parse_results/trimmed/insurance_hist_items_CLAIMS.csv");
    claims_outfile << "lowval,midpoint,highval,CLAIMS_idx,count,avg_duration,avg_loss\n";
    
    for (int i=0; i<NUM_BUCKETS; i++) {
        for (int j=0; j<CLAIMS_size; j++) {
            claims_outfile << insurance_CLAIMS[i][j].lowval << "," \
                           << insurance_CLAIMS[i][j].midpoint << ","\
                           << insurance_CLAIMS[i][j].highval << ","\
                           << j << "," \
                           << insurance_CLAIMS[i][j].count << ","\
                           << insurance_CLAIMS[i][j].avg_duration << ","\
                           << insurance_CLAIMS[i][j].avg_loss << "\n";
        }
    }
    claims_outfile.close();

    // Repeat for CAUGHT
    ofstream caught_outfile;
    caught_outfile.open("../scripts/parse_results/trimmed/insurance_hist_items_CAUGHT.csv");
    caught_outfile << "lowval,midpoint,highval,CAUGHT_idx,count,avg_duration,avg_loss\n";
    
    for (int i=0; i<NUM_BUCKETS; i++) {
        for (int j=0; j<CAUGHT_size; j++) {
            caught_outfile << insurance_CAUGHT[i][j].lowval << "," \
                           << insurance_CAUGHT[i][j].midpoint << ","\
                           << insurance_CAUGHT[i][j].highval << ","\
                           << j << "," \
                           << insurance_CAUGHT[i][j].count << ","\
                           << insurance_CAUGHT[i][j].avg_duration << ","\
                           << insurance_CAUGHT[i][j].avg_loss << "\n";
        }
    }
    caught_outfile.close();
}

void insurance_step(Line line) {
    
    if (line.i_init <= 0) {
        // This is not a relevant trial, since the insurer did not participate
        // and hence this trial should not be considered.
        // MAKE SURE THIS IS CLEARLY STATED IN THE PAPER WHEN REPORTING ON THIS DATA
        return;
    }

    assert(line.i_init < ASSUMED_MAX_I_INIT); // like max i_init (up to MANDATE=0.5): 43845463
    int i_init_bucket = line.i_init / I_INIT_DIVISOR;
    // cout << line.PREMIUM << " " << line.i_init << " " << i_init_bucket << " " << line.CLAIMS << " " << line.CAUGHT << endl;


    int claims_bucket = (int) round((line.CLAIMS - 0.1) * 10);
    int caught_bucket = (int) round(line.CAUGHT * 10);

    double d_init_original = (double) line.d_init / (double) (1 - line.MANDATE);
    double loss = (double) line.d_end / (double) d_init_original;

    insurance_CLAIMS[i_init_bucket][claims_bucket].count++;
    insurance_CLAIMS[i_init_bucket][claims_bucket].update_avg_loss(loss);
    insurance_CLAIMS[i_init_bucket][claims_bucket].update_avg_duration(line.final_iter);

    insurance_CAUGHT[i_init_bucket][caught_bucket].count++;
    insurance_CAUGHT[i_init_bucket][caught_bucket].update_avg_loss(loss);
    insurance_CAUGHT[i_init_bucket][caught_bucket].update_avg_duration(line.final_iter);
}

void insurance_init() {

    for (int i=0; i<NUM_BUCKETS; i++) {
        for (int j=0; j<CLAIMS_size; j++) {
            insurance_CLAIMS[i][j].lowval   = i * I_INIT_DIVISOR;
            insurance_CLAIMS[i][j].midpoint = insurance_CLAIMS[i][j].lowval + I_INIT_DIVISOR/2;
            insurance_CLAIMS[i][j].highval  = insurance_CLAIMS[i][j].lowval + I_INIT_DIVISOR - 1;
            insurance_CLAIMS[i][j].count        = 0;
            insurance_CLAIMS[i][j].avg_loss     = 0;
            insurance_CLAIMS[i][j].avg_duration = 0;
        }
    }

    for (int i=0; i<NUM_BUCKETS; i++) {
        for (int j=0; j<CAUGHT_size; j++) {
            insurance_CAUGHT[i][j].lowval   = i * I_INIT_DIVISOR;
            insurance_CAUGHT[i][j].midpoint = insurance_CAUGHT[i][j].lowval + I_INIT_DIVISOR/2;
            insurance_CAUGHT[i][j].highval  = insurance_CAUGHT[i][j].lowval + I_INIT_DIVISOR - 1;
            insurance_CAUGHT[i][j].count        = 0;
            insurance_CAUGHT[i][j].avg_loss     = 0;
            insurance_CAUGHT[i][j].avg_duration = 0;
        }
    }

    insurance_end();
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
                cout << ifilename << endl;
                ifstream infile(ifilename);

                string linestr;
                getline(infile, linestr); // throwaway first line

                while (getline(infile, linestr)) {    
                    Line line = linestr_to_line(linestr);
                    
                    // Do you in-loop steps here
                    insurance_step(line);
                }

                // Write intermediate results
                insurance_end();
            }
        }
    }


    return 0;
}