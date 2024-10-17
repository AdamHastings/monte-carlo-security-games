#!/usr/bin/python3
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import sys
import os
import os.path
from matplotlib.lines import Line2D
import matplotx


def trillion_formatter(x, pos):
    return "$%.0fT" % (x / 1E12)


def plot_cumulative_assets(df):    

    df['d_cumulative_assets'] = df['d_cumulative_assets'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=int, sep=','))
    df['a_cumulative_assets'] = df['a_cumulative_assets'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=int, sep=','))
    df['i_cumulative_assets'] = df['i_cumulative_assets'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=int, sep=','))

    # plt.clf()
    # plt.figure(figsize=(1,1))

    with plt.style.context(matplotx.styles.dufte):

        # plt.figure(figsize=(7,2))
        fig, ax = plt.subplots()
        # plt.figure(figsize=(4,3))
        fig.set_size_inches(5,3.5)
        ax.yaxis.set_major_formatter(trillion_formatter)

        df.final_iter = df.final_iter.astype(int)


        for label, c, l in zip(['defenders', 'attackers', 'insurers'], ['b', 'r', 'y'], ['--', '-', '-.']):

            frame = label[0] + "_cumulative_assets"

            for i in range(len(df[frame])):
                plt.plot(df[frame][i], color=c, label=label, alpha=0.05)


            # cumulative_assets_5th_pct = []
            # cumulative_assets_median = []
            # cumulative_assets_95th_pct = []

            # # consider shortest run instead?
            # # or perhaps even median run?
            # length = int(df[frame].map(lambda x : len(x)).median())


            # for i in range(length):
            #     col = [x[i] for x in df[frame] if i < len(x)]

            #     cumulative_assets_5th_pct.append(np.percentile(col, 5))
            #     cumulative_assets_median.append(np.percentile(col, 50))
            #     cumulative_assets_95th_pct.append(np.percentile(col, 95))

            # x = range(length)
            


            # plt.fill_between(x, cumulative_assets_5th_pct, cumulative_assets_95th_pct, color=c, alpha=0.5, edgecolor='none')
            # plt.plot(cumulative_assets_median, color=c, label=label, linestyle=l)


        

        plt.ylabel("cumulative wealth")
        plt.xlabel("timestep")
        # plt.legend()
        # Creating custom legend handles
        custom_handles = [
            Line2D([0], [0], color='b', lw=2),
            Line2D([0], [0], color='r', lw=2),
            Line2D([0], [0], color='y', lw=2)
        ]

        # Creating custom legend labels
        plt.legend(custom_handles, ['Defenders', 'Attackers', 'Insurers'], loc='upper left', framealpha=1.0)
        # ax.set_xticks(np.arange(0,5000,1000))

        basetitle = "cumulative_assets"
        dirname = "figures"
        subdirname = df['folder'][0]
        path = dirname + '/' + subdirname 
        
        if not os.path.isdir(path):
            os.mkdir(path)

        plt.gca().xaxis.grid(True)
        plt.tight_layout()

        plt.axhline(1 * 10**11, 0, 3000)

        # plt.show()
        plt.savefig(path + '/' + basetitle + '.png')
        plt.savefig(path + '/' + basetitle + '.pdf')



if __name__=="__main__":
    
    if (len(sys.argv) == 2):
        filename = sys.argv[1]
    else:
        print("Incorrect number of args! Example:")
        print("$ python3 run_all.py <path_to_log_file.csv>")
        sys.exit(1)

    df = pd.read_csv(filename, header=0)
    filename = filename.replace("../logs/", "")
    filename = filename.replace(".csv", "")
    df['folder'] = filename

    plot_cumulative_assets(df)