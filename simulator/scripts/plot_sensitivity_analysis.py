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
    "POSTURE" : 0.28,
    "POSTURE_NOISE" : 0.1,
    "NUM_QUOTES" : 10,
    "LOSS_RATIO" : 0.8, # Separate experiment,
    "RETENTION_REGRESSION_FACTOR" : 25.0,
    "ATTACKS_PER_EPOCH" : 100,
    "CTA_SCALING_FACTOR" : 0.85714285714,
    "DEPRECIATION" : 0.4,
    "INVESTMENT_SCALING_FACTOR" : 25,
}

percentage_vars = {
    "INEQUALITY",
    "POSTURE",
    "DEPRECIATION",
    "LOSS_RATIO"
}

non_percentage_vars = {
    "RANSOM_B0",
    "RANSOM_B1",
    "RECOVERY_COST_BASE",
    "RECOVERY_COST_EXP",
    "POSTURE_NOISE",
    "NUM_QUOTES",
    "RETENTION_REGRESSION_FACTOR",
    "ATTACKS_PER_EPOCH",
    "CTA_SCALING_FACTOR",
    "INVESTMENT_SCALING_FACTOR",
}


labels = {
    "INEQUALITY" :r'$s_I$',
    "RANSOM_B0" :r'$\beta_{0,ransom}$',
    "RANSOM_B1" :r'$\beta_{1,ransom}$',
    "RECOVERY_COST_BASE" :r'$c_{recovery}$',
    "RECOVERY_COST_EXP" :r'$n_{recovery}$',
    "POSTURE" :r'$\mu_p$',
    "POSTURE_NOISE" :r'$\sigma_p$',
    "NUM_QUOTES" :r'$Q$',
    "LOSS_RATIO" : r'$LR$',
    "RETENTION_REGRESSION_FACTOR" :r'$\beta_{1,retention}$',
    "ATTACKS_PER_EPOCH" :r'$K$',
    "CTA_SCALING_FACTOR" :r'$s_c$',
    "DEPRECIATION" :r'$\lambda$',
    "INVESTMENT_SCALING_FACTOR" :r'$s_p$',
}

def plot_sensitivity_analysis():

    assert(len(labels) == len(baseline_vals))
    assert(len(baseline_vals) == len(percentage_vars) + len(non_percentage_vars))

    with plt.style.context(matplotx.styles.dufte):

        # Override gridline and label colors to black
        plt.rcParams.update({
            'grid.color': 'black',  # Set gridlines to black
            'xtick.color': 'black',  # Set x-axis tick labels to black
            'ytick.color': 'black',  # Set y-axis tick labels to black
            'axes.labelcolor': 'black',  # Set axis labels to black
            'axes.edgecolor': 'black',  # Set the axis edge color to black
            'text.color': 'black'  # Default text color
        })
        
        plt.figure(figsize=(5,3))

        plt.rcParams['text.usetex'] = True

    
        reldir = "../logs/sweeps"
        for foldername in os.listdir("../logs/sweeps/"): # consider doing MAX_ITERATIONS=5000 as well
            print(foldername)
            folderpath = reldir + "/" + foldername

            plt.clf()
            for var in percentage_vars:
                filename = "sweep_" + var + ".csv"
                # path = "../logs/sweeps/"
                filepath = folderpath + "/" + filename
                # print("  " + filepath)

                try:
                    df = pd.read_csv(filepath)
                except:
                    print("     " + filepath + " does not exist!")
                    continue

                var = filename[6:-4] # trim off sweep_ and .csv
                # print(var)


                if len(df) < 100:
                    print("     " + var + " log empty!")
                    continue

                if not var in df:
                    print("     " + var + " key not found!")
                    continue


                # sort by value
                df = df.sort_values(by=[var])
                # print(df)
                
                df['loss'] = 1 - df['d_end'] / df['d_init'] # == (d_init - d_end) / d_init
                assert(df[var].max() <= 1)
                assert(df[var].min() >= 0)


                plt.plot(df[var], df['loss'], label=labels[var])


            ax = plt.gca()
            ax.grid(which='major', axis='x')
            # plt.xticks((0,1,2), ("0x", "1x\n(baseline value)", "2x"))
            plt.xlabel("parameter value")
            plt.ylabel("loss (lower is better)")
            plt.ylim(0,1)
            plt.xlim(0,1)
            # plt.legend()
            matplotx.line_labels()  # line labels to the right
            plt.gca().xaxis.grid(True)
            plt.tight_layout()
            plt.savefig("figures/sensitivity_analysis/sensitivity_analysis_" + foldername + "_percentage_vars.pdf")
            plt.savefig("figures/sensitivity_analysis/sensitivity_analysis_" + foldername + "_percentage_vars.png")


            ## Now do non-percentage vars
            plt.clf()
            plt.figure(figsize=(5,3))
            for var in non_percentage_vars:
                filename = "sweep_" + var + ".csv"
                # path = "../logs/sweeps/"
                filepath = folderpath + "/" + filename
                # print("  " + filepath)

                try:
                    df = pd.read_csv(filepath)
                except:
                    print("     " + filepath + " does not exist!")
                    continue

                var = filename[6:-4] # trim off sweep_ and .csv
                # print(var)


                if len(df) < 100:
                    print("     " + var + " log empty!")
                    continue

                if not var in df:
                    print("     " + var + " key not found!")
                    continue


                # sort by value
                df = df.sort_values(by=[var])
                # print(df)
                
                df['loss'] = 1 - df['d_end'] / df['d_init'] # == (d_init - d_end) / d_init
                df[var] = df[var] / baseline_vals[var] # Should normalize df[val] to be within range [0,2]
                assert(df[var].max() <= 2)
                assert(df[var].min() >= 0)


                plt.plot(df[var], df['loss'], label=labels[var])

            ax = plt.gca()
            ax.grid(which='major', axis='x')
            plt.xticks((0,1,2), ("0x", "1x\n(baseline value)", "2x"))
            plt.xlabel("parameter value")
            plt.ylabel("loss (lower is better)")
            plt.ylim(0,1)
            # plt.legend()
            matplotx.line_labels()  # line labels to the right
            plt.gca().xaxis.grid(True)
            plt.tight_layout()
            plt.savefig("figures/sensitivity_analysis/sensitivity_analysis_" + foldername + "_non_percentage_vars.pdf")
            plt.savefig("figures/sensitivity_analysis/sensitivity_analysis_" + foldername + "_non_percentage_vars.png")



if __name__=="__main__":
    assert(len(sys.argv) == 1)
    plot_sensitivity_analysis()