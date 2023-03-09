#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <sstream>
#include <math.h>
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

struct LineStruct {
    // double mandate;
    // double tax;
    // double premium;
    int ATTACKERS;
    int INEQUALITY;
    int EFFICIENCY;
    int EFFORT;
    int PAYOFF;
    int CAUGHT;
    int CLAIMS;
    string full_line;
    int idx;

    bool operator < (const LineStruct& lineB) const
    {
        // if (CLAIMS != lineB.CLAIMS) {
            // return CLAIMS < lineB.CLAIMS;
        // } else if (CAUGHT != lineB.CAUGHT){
            // return CAUGHT < lineB.CAUGHT;
        // } else if (PAYOFF != lineB.PAYOFF) {
            // return PAYOFF < lineB.PAYOFF;
        // } else if (EFFORT != lineB.EFFORT) {
            // return EFFORT < lineB.EFFORT;
        // } else if (EFFICIENCY != lineB.EFFICIENCY) {
            // return EFFICIENCY < lineB.EFFICIENCY;
        // } else if (INEQUALITY != lineB.INEQUALITY) {
            // return INEQUALITY < lineB.INEQUALITY;
        // } else {
            // return ATTACKERS < lineB.ATTACKERS;
        // }

        if      (ATTACKERS  != lineB.ATTACKERS ) return ATTACKERS  < lineB.ATTACKERS;
        else if (INEQUALITY != lineB.INEQUALITY) return INEQUALITY < lineB.INEQUALITY;
        else if (EFFICIENCY != lineB.EFFICIENCY) return EFFICIENCY < lineB.EFFICIENCY;
        else if (EFFORT     != lineB.EFFORT    ) return EFFORT     < lineB.EFFORT;
        else if (PAYOFF     != lineB.PAYOFF    ) return PAYOFF     < lineB.PAYOFF;
        else if (CAUGHT     != lineB.CAUGHT    ) return CAUGHT     < lineB.CAUGHT;
        else                                     return CLAIMS     < lineB.CLAIMS;
    }
};
 

int main(int argc, char* argv[]) {

    // string filename = "../split/MANDATE=0.0_TAX=0.0_PREMIUM=0.0.csv";
    string filename = "../split/test.csv";
    ifstream infile(filename);

    string line;

    string ofilename = "../sorted/sorted_test.csv";
    ofstream out;
    out.open(ofilename);


    getline(infile, line); // throwaway first line
    string firstline = line;
    // cout << firstline << endl;

    out << firstline << endl;

    vector<LineStruct> lines;

    int count = 0;
    while(getline(infile, line)) {
        vector<string> split = splitline(line);

        LineStruct ls;
        ls.idx = count;
        ls.full_line  = line;
        ls.ATTACKERS  = (int) round(stod(split[1]) * 10);
        ls.INEQUALITY = (int) round(stod(split[2]) * 10);
        ls.EFFICIENCY = (int) round(stod(split[4]) * 10);
        ls.EFFORT     = (int) round(stod(split[5]) * 10);
        ls.PAYOFF     = (int) round(stod(split[6]) * 10);
        ls.CAUGHT     = (int) round(stod(split[7]) * 10);
        ls.CLAIMS     = (int) round(stod(split[8]) * 10);

        count += 1;
        // cout << ls.full_line << endl;

        if (ls.full_line == "") {
            cout << "it's empty!" << endl;
            while(1);
        }
        
        // if (ls.CAUGHT == 0) {
        //     cout << ls.ATTACKERS << ", " << ls.ATTACKERS << ", " << ls.INEQUALITY << ", " << ls.EFFICIENCY << ", " << ls.EFFORT << ", " << ls.PAYOFF << ", " << ls.CAUGHT << ", " << ls.CLAIMS << endl;
        // }
        // cout << split[7] << " - " << stod(split[7]) << " - " << round(stod(split[7]) * 10) << " -> " << ls.CAUGHT << endl;

        lines.push_back(ls);
    }

    cout << "All lines loaded. Attempting to sort..." << endl;

    for (auto ls : lines) {
        if (ls.full_line == "") {
            cout << ls.ATTACKERS << ", " << ls.INEQUALITY << ", " << ls.EFFICIENCY << ", " << ls.EFFORT << ", " << ls.PAYOFF << ", " << ls.CAUGHT << ", " << ls.CLAIMS << endl;
            cout << ls.full_line << endl;
        }
    }


    sort(lines.begin(), lines.end());

    cout << "done sorting" << endl;

    
    for (auto ls : lines) {
        // cout << line.full_line << endl;
        out << ls.full_line << endl;
    }

    out.close();

    



    return 0;
}