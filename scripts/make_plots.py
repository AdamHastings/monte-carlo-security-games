#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.lines import Line2D
from termcolor import colored
from matplotlib.ticker import AutoMinorLocator, FormatStrFormatter



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
        if (m == 1 or m == 0.9):
            continue

        mands = df.loc[df['SEC_INVESTMENT'] == m]
        convergences = mands['final_iter'].to_numpy()
        convergences = convergences - 50

        N = 100000
        X2 = np.sort(convergences)
        F2 = np.array(range(N))/float(N) * 100

        plt.step(X2, F2, label=str(int(m * 100)) + "%")

    plt.ylim(60,100)
    plt.title("CDF of convergence iterations")
    # plt.yscale("log")
    plt.xlim(left=0, right=1250)
    plt.minorticks_on()
    plt.grid(True, which='both')
    plt.xlabel("Iteration")
    plt.ylabel("Percent of games")
    plt.legend(loc="lower right", title="Mandate:")
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
        
        N = 100000
        X2 = np.sort(ratios)
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

        print("---------------")
        # F2 = 100 - F2
        plt.plot(X2, F2, label=str(int(m * 100)) + "%")


        
    plt.title("CDF of percent decrease in assets")
    plt.xlabel("Percent decrease in assets (mandated spending + losses from attacks)")
    plt.ylabel("Percent of simulations")
    plt.xlim(0,100)
    plt.ylim(60,100)
    # plt.xaxis.set_minor_locator(AutoMinorLocator())
    # plt.xaxis.set_minor_formatter(FormatStrFormatter("%.3f"))
    print(mandates)
    plt.minorticks_on()
    plt.grid(True, which='both')
    plt.legend(loc="lower right", title="Mandate:")
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