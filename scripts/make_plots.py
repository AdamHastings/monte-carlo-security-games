#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.lines import Line2D
from termcolor import colored


def crossover_hist(df):
    
    # fig,a =  plt.subplots(2,5,sharey=True)

    custom_lines = []
    mandates = sorted(df['SEC_INVESTMENT'].unique())
    for i,m in enumerate(mandates):
        if (m == 1):
            continue


        mands = df.loc[df['SEC_INVESTMENT'] == m]
        crossovers = mands['crossover'].to_numpy()
        # crossovers = np.where(crossovers == -1, -100, crossovers)
        no_crossovers = np.count_nonzero(crossovers == -1)
        print(no_crossovers)
        # print(crossovers.shape)
        # row = i//5
        # col = i%5
        # plt.hist(crossovers, bins=100, cumulative=True,  histtype='step', label=str(m * 100) + "% mandate", density=True)
        # custom_lines.append(Line2D([0], [0], lw=4))
        N = 100000
        X2 = np.sort(crossovers)
        F2 = np.array(range(N))/float(N) * 100 * (N - no_crossovers)/N

        plt.step(X2, F2, label=str(int(m * 100)) + "%")
        
        # patches[0].set_fc('r')
        # a[row, col].set_title("Mandate = " + str(m * 100) + "%")
    # plt.yscale("log")
    plt.title("CDF of crossover iterations")
    plt.legend(loc="upper right", title="Mandated security spending")
    plt.xlabel("Iteration")
    plt.ylabel("Percent of games")
    # plt.xlabel("Crossover iteration (far-left red bar means no crossover)")
    # plt.ylabel("Count")
    plt.show()  

def converge_hist(df):
    # fig,a =  plt.subplots(2,5,sharey=True, sharex=True)

    mandates = sorted(df['SEC_INVESTMENT'].unique())
    for i,m in enumerate(mandates):
        if (m == 1):
            continue

        mands = df.loc[df['SEC_INVESTMENT'] == m]
        convergences = mands['final_iter'].to_numpy()
        convergences = convergences - 50

        # plt.hist(convergences, bins=100,  cumulative=True,  histtype='step', label=str(m * 100) + "% mandate", density=True)
        N = 100000
        X2 = np.sort(convergences)
        F2 = np.array(range(N))/float(N) * 100

        plt.step(X2, F2, label=str(m * 100) + "% mandate")

    # plt.ylim(0,10000)
    plt.title("CDF of convergence iterations")
    # plt.yscale("log")
    plt.xlabel("Iteration")
    plt.ylabel("Percent of games")
    plt.legend()
    # plt.tight_layout()
    plt.show()  

def loss_ratio_hist(df):
    
    # fig, a =  plt.subplots(2,5,sharey=True,sharex=True)

    mandates = sorted(df['SEC_INVESTMENT'].unique())

    custom_lines = []
    for i,m in enumerate(mandates):
        if (m == 1):
            continue

        print("*********---------------")

        # print(m)
        mands = df.loc[df['SEC_INVESTMENT'] == m]

        d_inits = mands['d_init'].to_numpy() / (1 - float(m))
        # print(d_inits)
        d_ends = mands['d_end'].to_numpy()

        convergences = mands['final_iter'].to_numpy()
        convergences = convergences - 50


        ratios = []
        for start,finish in zip(d_inits, d_ends):
            ratios.append(((start - finish)/start) * 100)
        
        # row = i//5
        # col = i%5
        # print(row, col)
        # a[0, 0].hist(np.zeros(100), bins=100)
        # plt.hist(ratios, bins=np.linspace(0,100,100),  cumulative=True,  histtype='step', label=str(m * 100) + "% mandate", density=True)
        # custom_lines.append(Line2D([0], [0], color=cmap(m), lw=4))
        # a[row, col].set_title("Mandate = " + str(m * 100) + "%")
        # a[row, col].set_ylim(bottom=1)
        # a[row, col].yscale("log")

        N = 100000
        X2 = np.sort(ratios)
        # X2 = X2[X2 != m * 100]
        # N = X2.shape[0]
        # print(N)

        # X3 = np.delete(X2,np.arange(100))
        # print(X3.shape)
        F2 = np.array(range(N))/float(N) * 100
        
        # F2 = np.delete(F2,np.arange(100))
        count = 0
        for i in X2:
            if int(round(i)) != int(round(m * 100)):
                break
            else:
                count += 1

        print("count: " + str(count))
        X2 = np.delete(X2, np.arange(count))
        F2 = np.delete(F2, np.arange(count))
        # print(X2[0])
        # print(m * 100)
        # print(X2)
        # print(F2)
        print("---------------")
        # F2 = 100 - F2
        plt.plot(X2, F2, label=str(int(m * 100)) + "%")

        # sorted_ratios = sorted(ratios)
        # print(sorted_ratios)
        # cdf = []
        # for i,r in enumerate(sorted_ratios):
        #     if i == 0:
        #         cdf.append(r)
        #     else:
        #         cdf.append(cdf[i-1] + r)

        # x = np.arange(len(cdf))
        # # print(x)
        # # print(cdf)
        # plt.step(x, cdf)
        # plt.show()
            
    # plt.ylim(0, 1*10**6)

    # plt.yscale("log")
    # plt.ylim(0,10000)
    plt.title("CDF of percent of assets lost")
    plt.xlabel("Percent of assets lost (mandated spending + losses from attacks)")
    plt.ylabel("Percent of games")
    plt.xlim(0,100)
    plt.ylim(0,100)
    print(mandates)
    plt.minorticks_on()
    plt.legend(loc="lower right", title="Mandate:")
    plt.grid(True, which='both')
    # plt.tight_layout()
    # plt.legend(loc="lower right")
    plt.show()  

def make_plots(df):
    # print(colored('  [+] Making crossover histogram', 'green'))
    # crossover_hist(df)
    # print(colored('  [+] Making convergence histogram', 'green'))
    # converge_hist(df)
    print(colored('  [+] Making loss ratio histogram', 'green'))
    loss_ratio_hist(df)