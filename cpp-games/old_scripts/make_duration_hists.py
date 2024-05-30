#!/usr/bin/python3
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt


# For each MANDATE
for m in ["0.0", "0.1", "0.2", "0.3", "0.4", "0.5"]:
    

    # each of these will be a unique plot
    plt.figure()

    # For each PREMIUM:
    for p in  ["0.0", "0.1", "0.2", "0.3", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9", "1.0"]:

        histbin = [0] * 1000

        # For t in TAX
        for t in ["0.0"]: # ["0.0", "0.1", "0.2", "0.3", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9", "1.0"]:        
            ofname = '../trimmed/MANDATE=' + m + "_TAX=" + t + "_PREMIUM=" + p + ".csv"
            print(ofname)

            df = pd.read_csv(ofname)
            df = df[df['final_iter'] <= 50] # 50 is the threshold for no action # should have already been trimmed...
            hist_array = df['final_iter'].value_counts(sort=False).sort_index().values

            plt.plot(hist_array, label="PREMIUM=" + p)

    plt.ylim(None,None)
    plt.ylabel("count")
    plt.xlabel("duruation (# iterations)")
    plt.legend()
    plt.title=("MANDATE=" + m)
    plt.savefig("figures/durations/MANDATE=" + m + ".pdf")
    plt.savefig("figures/durations/MANDATE=" + m + ".png")

    plt.ylim(0, 5500)
    plt.savefig("figures/durations/MANDATE=" + m + "_ylim.pdf")
    plt.savefig("figures/durations/MANDATE=" + m + "_ylim.png")


            

            
