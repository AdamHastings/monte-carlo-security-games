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

string getMANDATE(string filename) {
    size_t start = filename.find("=") + 1;
    string output = filename.substr(start, 3);
    return output;
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        cerr << "no input file provided!" << endl;
        return -1;
    }        

    string filename = argv[1];
    ifstream infile(filename);
    if (!infile.good()) {
        cerr << "bad input filename" << endl;
        return -1;
    }


    string mandate = getMANDATE(filename);

    string line;
    int count = 0;
    int splitcount = 0;
    getline(infile, line); // throwaway first line
    string firstline = line;

    ofstream outfile;
    string outfilename = "../split/";
    const int TAX_size = 11;
    const int PREMIUM_size=11;
    ofstream out[TAX_size][PREMIUM_size];

    // if( remove(outfilename.c_str()) != 0 ) {
    //     cout << "Error deleting file" << endl;
    // } else {
    //     cout << "File successfully deleted" << endl;
    // }


    for (int i=0; i<TAX_size; i++) {
        for (int j=0; j<PREMIUM_size; j++) {
            string taxstr = i<10 ? "0." + to_string(i): "1.0";
            string premstr = j<10 ? "0." + to_string(j): "1.0";
            string ofilename = "../split/MANDATE=" + mandate + "_TAX=" + taxstr + "_PREMIUM=" + premstr + ".csv";
            out[i][j].open(ofilename);
            out[i][j] << firstline << endl;
        }
    }

    // outfile.open(outfilename, std::ios_base::app); // append instead of overwrite
    // outfile << line << endl;

    while (getline(infile, line)) {       
        count += 1; 
        // First parse the line
        vector<string> split = splitline(line);

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
        out[taxidx][premidx] << line << endl;
    }

    // outfile.close();

    for (int i=0; i<TAX_size; i++) {
        for (int j=0; j<PREMIUM_size; j++) {
            out[i][j].close();
        }
    }

    return 0;
}