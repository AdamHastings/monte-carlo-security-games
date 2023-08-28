#!/usr/bin/python3
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt


# For each PREMIUM:
for p in  ["0.0", "0.1", "0.2", "0.3", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9", "1.0"]:


    # each of these will be a unique plot
    plt.figure(1)
    plt.clf()

    plt.figure(2)
    plt.clf()


    # For each MANDATE
    for m in ["0.0", "0.1", "0.2", "0.3", "0.4", "0.5"]:

        # For t in TAX
        # Only considering TAX=0.0 for now
        for t in ["0.0"]: # ["0.0", "0.1", "0.2", "0.3", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9", "1.0"]:   

            ofname = '../trimmed/MANDATE=' + m + "_TAX=" + t + "_PREMIUM=" + p + ".csv"
            print(ofname)    

            df = pd.read_csv(ofname)

            # initial * (1 - MANDATE) = d_init
            # initial = d_init / (1 - MANDATE)
            # the amount Defenders have *before* any mandate is applied
            df['d_init_init'] = df['d_init'] / (1 - df['MANDATE'])
            df['loss'] = df['d_end'] / df['d_init_init']
            df['efficiency'] = 1 - df['loss']


            plt.figure(1)
            hist, bins = np.histogram(df['loss'], bins = 100, density = True )
            # print(bins)
            dx = bins[1] - bins[0]
            cdf = np.cumsum(hist)*dx

            # print(cdf)
            plt.plot(cdf, label=str(int(m * 100)) + "%")

            plt.figure(2)
            plt.plot(cdf, label="MANDATE=" + m) # TODO

    plt.figure(1)
    plt.title("PREMIUM="+p)
    plt.legend()
    plt.minorticks_on()
    plt.grid(True, which='both')
    plt.xlabel("percent of games")
    plt.ylabel("loss")
    plt.tight_layout() 
    plt.savefig("figures/cdfs/pdf/PREMIUM=" + p + ".pdf")
    plt.savefig("figures/cdfs/png/PREMIUM=" + p + ".png")


    plt.figure(2)
    plt.title("PREMIUM="+p)
    plt.legend()
    plt.minorticks_on()
    plt.grid(True, which='both')
    plt.ylabel("percent of simulations")
    plt.xlabel("percent decrease in assets (mandated spending + losses from attacks)")
    plt.tight_layout() 
    plt.savefig("figures/old_cdfs/pdf/PREMIUM=" + p + ".pdf")
    plt.savefig("figures/old_cdfs/png/PREMIUM=" + p + ".png")



