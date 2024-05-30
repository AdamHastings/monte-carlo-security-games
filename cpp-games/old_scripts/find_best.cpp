#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char* argv[]) {
    
    // int range[6] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5};
    string mrange[3] = {"0.0", "0.1", "0.2"};


    const int TAX_size = 11;
    const int PREMIUM_size=11;

    for (int i=0; i<TAX_size; i++) {
        for (int j=0; j<PREMIUM_size; j++) {

            
            string taxstr = i<10 ? "0." + to_string(i): "1.0";
            string premstr = j<10 ? "0." + to_string(j): "1.0";
            // string ofilename = "../split/MANDATE=" + mandate + "_TAX=" + taxstr + "_PREMIUM=" + premstr + ".csv";
            // out[i][j].open(ofilename);
            // out[i][j] << firstline << endl;
            cout << "TAX=" << taxstr << "_PREMIUM=" << premstr << ": " << flush;

            vector<ifstream> ins;

            for (string m : mrange) {
                string filename = "../sorted/MANDATE=" + m + "_TAX=" + taxstr + "_PREMIUM=" + premstr + ".csv";
                // cout << filename << endl;
                ifstream in;
                in.open(filename);
                ins.push_back(in);
            }

            cout << "Loaded! Parsing..." << flush;

            vector<string> lines;
            int linecount = 0;

            // for (ifstream in : ins) {
            //     getline(in, )
            // }

            for (int m=0; m<ins.size(); m++) {
                string line
                getline()
            }

            // TODO parse

            cout << "done!" << endl;
        }
    }
}