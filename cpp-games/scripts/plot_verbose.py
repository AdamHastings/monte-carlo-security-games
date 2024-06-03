#!/usr/bin/python3
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt


def trillion_formatter(x, pos):
    return "$%.0fT" % (x * 1E3 / 1E12)


def plot_verbose(df):
    

    df['d_cumulative_assets'] = df['d_cumulative_assets'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=int, sep=','))
    df['a_cumulative_assets'] = df['a_cumulative_assets'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=int, sep=','))
    df['i_cumulative_assets'] = df['i_cumulative_assets'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=int, sep=','))

    df.final_iter = df.final_iter.astype(int)

    fig, ax = plt.subplots()


    for label, c, l in zip(['defenders', 'attackers', 'insurers'], ['b', 'r', 'y'], ['--', '-', '-.']):

        frame = label[0] + "_cumulative_assets"

        cumulative_assets_5th_pct = []
        cumulative_assets_median = []
        cumulative_assets_95th_pct = []

        longest_run = df[frame].map(lambda x : len(x)).max()


        for i in range(longest_run):
            col = [x[i] for x in df[frame] if i < len(x)]

            cumulative_assets_5th_pct.append(np.percentile(col, 5))
            cumulative_assets_median.append(np.percentile(col, 50))
            cumulative_assets_95th_pct.append(np.percentile(col, 95))

        x = range(longest_run)
        


        plt.fill_between(x, cumulative_assets_5th_pct, cumulative_assets_95th_pct, color=c, alpha=0.5, edgecolor='none')
        plt.plot(cumulative_assets_median, color=c, label=label, linestyle=l)


    ax.yaxis.set_major_formatter(trillion_formatter)

    plt.ylabel("cumulative wealth")
    plt.xlabel("time")
    plt.legend()

    basetitle = "cumulative_assets"

    plt.savefig("figures/verbose/" + basetitle + ".png")
    plt.savefig("figures/verbose/" + basetitle + ".pdf")



if __name__=="__main__":
    df = pd.read_csv("../logs/fullsize_short_verbose.csv", header=0)
    plot_verbose(df)