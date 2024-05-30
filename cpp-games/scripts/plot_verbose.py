#!/usr/bin/python3
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt


def plot_verbose(df):
    
    num_figs = 20 # this is enough to get a sense of what's going on

    for i in range(num_figs):

        row = df.iloc[i]
        defender_assets = np.array(row['d_cumulative_assets'].replace('[','').replace(']','').split(',')).astype(int)
        attacker_assets = np.array(row['a_cumulative_assets'].replace('[','').replace(']','').split(',')).astype(int)
        insurer_assets  = np.array(row['i_cumulative_assets'].replace('[','').replace(']','').split(',')).astype(int)

        plt.figure(i)
        plt.plot(defender_assets, color='b')
        plt.plot(attacker_assets, color='r')
        plt.plot(insurer_assets, color='y')
        plt.ylabel("cumulative wealth")
        plt.xlabel("time")

        basetitle = "cumulative_assets_" + str(i)

        plt.savefig("figures/verbose/png/" + basetitle + ".png")
        plt.savefig("figures/verbose/pdf/" + basetitle + ".pdf")



if __name__=="__main__":
    df = pd.read_csv("../logs/fullsize_short_verbose.csv", index_col=0, header=0)
    plot_verbose(df)