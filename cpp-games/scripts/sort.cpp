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


    if (argc != 2) {
        cerr << "Must provide input MANDATE! E.g. ./sort 0.2" << endl;
        return -1;
    }

    string mandate = argv[1];

    // string filename = argv[1];
    // ifstream infile(filename);
    // if (!infile.good()) {
    //     cerr << "bad input filename" << endl;
    //     return -1;
    // }

    // string filename = "../split/MANDATE=0.0_TAX=0.0_PREMIUM=0.0.csv";
    // string filename = "../split/MANDATE=0.2_TAX=0.0_PREMIUM=0.0.csv";

    const int TAX_size = 11;
    const int PREMIUM_size=11;

    for (int i=10; i<TAX_size; i++) {
        for (int j=8; j<PREMIUM_size; j++) {
            
            string taxstr = i<10 ? "0." + to_string(i): "1.0";
            string premstr = j<10 ? "0." + to_string(j): "1.0";
            string ifilename = "../split/MANDATE=" + mandate + "_TAX=" + taxstr + "_PREMIUM=" + premstr + ".csv";
            string ofilename = "../sorted/MANDATE=" + mandate + "_TAX=" + taxstr + "_PREMIUM=" + premstr + ".csv";
            cout << ifilename << ": " << std::flush;
        
            ifstream infile(ifilename);

            string line;

            ofstream out;
            out.open(ofilename);


            getline(infile, line); // throwaway first line
            string firstline = line;
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
                lines.push_back(ls);
            }

            cout << "Loaded. Sorting..." << std::flush;

            for (auto ls : lines) {
                if (ls.full_line == "") {
                    cout << ls.ATTACKERS << ", " << ls.INEQUALITY << ", " << ls.EFFICIENCY << ", " << ls.EFFORT << ", " << ls.PAYOFF << ", " << ls.CAUGHT << ", " << ls.CLAIMS << endl;
                    cout << ls.full_line << endl;
                }
            }


            sort(lines.begin(), lines.end());

            cout << "done! Writing..." << std::flush;

            
            for (auto ls : lines) {
                out << ls.full_line << endl;
            }

            out.close();

            cout << "done!" << endl;

        }
    }

    return 0;
}