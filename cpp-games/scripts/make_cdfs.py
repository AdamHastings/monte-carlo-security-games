#!/usr/bin/python3
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as mtick



# For each PREMIUM:
for p in  ["0.0", "0.1", "0.2", "0.3", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9", "1.0"]:


    # each of these will be a unique plot
    plt.figure(1)
    plt.clf()

    fig2 = plt.figure(2)
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
            plt.plot(cdf, label="MANDATE=" + str(int(float(m)*100)) + "%")

            plt.figure(2)
            

            d_inits = df['d_init'].to_numpy() / (1 - float(m))
            # print(d_inits)
            d_ends = df['d_end'].to_numpy()

            ratios = []
            for start,finish in zip(d_inits, d_ends):
                ratios.append(((start - finish)/start) * 100)
            
            N = len(ratios)
            # print("N = " + str(N))
            X2 = np.sort(ratios)
            F2 = np.array(range(N))/float(N) * 100
            # print(X2[0], X2[-1])
            
            # F2 = np.delete(F2,np.arange(100))
            count = 0
            for i in X2:
                if int(round(i)) != int(round(float(m) * 100)):
                    break
                else:
                    count += 1

            # print("count: " + str(count))
            X2 = np.delete(X2, np.arange(count))
            F2 = np.delete(F2, np.arange(count))

            # print("---------------")
            plt.step(X2, F2, label="MANDATE=" + str(int(float(m) * 100)) + "%")

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
    # plt.xlim(0,105)
    # plt.ylim(60,100)
    # plt.xaxis.set_major_formatter(mtick.PercentFormatter(decimals=0))
    # plt.yaxis.set_major_formatter(mtick.PercentFormatter(decimals=0))
    plt.tight_layout() 
    plt.savefig("figures/old_cdfs/pdf/PREMIUM=" + p + ".pdf")
    plt.savefig("figures/old_cdfs/png/PREMIUM=" + p + ".png")



