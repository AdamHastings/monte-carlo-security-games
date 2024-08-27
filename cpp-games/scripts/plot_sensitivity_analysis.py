import matplotlib.pyplot as plt
import matplotx
import numpy as np
import pandas as pd
import os
import sys


baseline_vals = {
    "INEQUALITY" : 0.001,
    "RANSOM_B0" : 792145,
    "RANSOM_B1" : 0.00121,
    "RECOVERY_COST_BASE" : 230123,
    "RECOVERY_COST_EXP" : 0.125,
    # "WEALTH" : ,
    "POSTURE" : 0.28,
    "POSTURE_NOISE" : 0.1,
    "NUM_QUOTES" : 10,
    # "LOSS_RATIO" : # Separate experiment,
    "RETENTION_REGRESSION_FACTOR" : 25.0,
    "ATTACKS_PER_EPOCH" : 100,
    "CTA_SCALING_FACTOR" : 0.85714285714,
    "DEPRECIATION" : 0.4,
    "INVESTMENT_SCALING_FACTOR" : 25,
}

def plot_sensitivity_analysis():
    with plt.style.context(matplotx.styles.dufte):

        plt.clf()
    
        reldir = "../logs/sweeps/MAX_ITERATIONS=100/"
        for filename in os.listdir(reldir): # consider doing MAX_ITERATIONS=5000 as well
            
            # path = "../logs/sweeps/"
            df = pd.read_csv(reldir + filename)

            var = filename[6:-4] # trim off sweep_ and .csv
            print(var)


            if len(df) < 100:
                print("  -- " + var + " log empty!")
                continue

            if not var in df:
                print("  ** " + var + " key not found!")
                continue


            # sort by value
            df = df.sort_values(by=[var])
            # print(df)
            
            df['loss'] = 1 - df['d_end'] / df['d_init'] # == (d_init - d_end) / d_init
            df[var] = df[var] / baseline_vals[var] # Should normalize df[val] to be within range [0,2]
            assert(df[var].max() <= 2)
            assert(df[var].min() >= 0)


            plt.plot(df[var], df['loss'], label=var)


        ax = plt.gca()
        ax.grid(which='major', axis='x')
        plt.xticks((0,1,2), ("0x", "1x\n(baseline value)", "2x"))
        plt.xlabel("parameter value")
        plt.ylabel("loss (lower is better)")
        plt.ylim(0,1)
        # plt.legend()
        matplotx.line_labels()  # line labels to the right

        plt.tight_layout()
        plt.savefig("figures/sensitivity_analysis/sensitivity_analysis.pdf")
        plt.savefig("figures/sensitivity_analysis/sensitivity_analysis.png")



if __name__=="__main__":
    assert(len(sys.argv) == 1)
    plot_sensitivity_analysis()