#!/usr/bin/python3
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import sys
import os
import os.path


def trillion_formatter(x, pos):
    return "$%.0fT" % (x / 1E12)


def plot_cumulative_assets(df):

    df['d_cumulative_assets'] = df['d_cumulative_assets'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=int, sep=','))
    df['a_cumulative_assets'] = df['a_cumulative_assets'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=int, sep=','))
    df['i_cumulative_assets'] = df['i_cumulative_assets'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=int, sep=','))

    plt.clf()
    

    df.final_iter = df.final_iter.astype(int)

    fig, ax = plt.subplots()


    for label, c, l in zip(['defenders', 'attackers', 'insurers'], ['b', 'r', 'y'], ['--', '-', '-.']):

        frame = label[0] + "_cumulative_assets"

        cumulative_assets_5th_pct = []
        cumulative_assets_median = []
        cumulative_assets_95th_pct = []

        # consider shortest run instead?
        # or perhaps even median run?
        length = int(df[frame].map(lambda x : len(x)).median())


        for i in range(length):
            col = [x[i] for x in df[frame] if i < len(x)]

            cumulative_assets_5th_pct.append(np.percentile(col, 5))
            cumulative_assets_median.append(np.percentile(col, 50))
            cumulative_assets_95th_pct.append(np.percentile(col, 95))

        x = range(length)
        


        plt.fill_between(x, cumulative_assets_5th_pct, cumulative_assets_95th_pct, color=c, alpha=0.5, edgecolor='none')
        plt.plot(cumulative_assets_median, color=c, label=label, linestyle=l)


    ax.yaxis.set_major_formatter(trillion_formatter)

    plt.ylabel("cumulative wealth")
    plt.xlabel("time")
    plt.legend()


    basetitle = "cumulative_assets"
    dirname = "figures"
    subdirname = df['folder'][0]
    path = dirname + '/' + subdirname 
    
    if not os.path.isdir(path):
        os.mkdir(path)

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