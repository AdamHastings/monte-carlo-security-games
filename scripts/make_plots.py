#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from termcolor import colored


def crossover_hist(df):
    
    # fig,a =  plt.subplots(2,5,sharey=True)

    mandates = sorted(df['SEC_INVESTMENT'].unique())
    for i,m in enumerate(mandates):
        if (m == 1):
            continue


        mands = df.loc[df['SEC_INVESTMENT'] == m]
        crossovers = mands['crossover'].to_numpy()
        crossovers = np.where(crossovers == -1, -100, crossovers)
        print(crossovers.shape)
        row = i//5
        col = i%5
        plt.hist(crossovers, bins=100, cumulative=True,  histtype='step', label=str(m * 100) + "% mandate", density=True)
        # patches[0].set_fc('r')
        # a[row, col].set_title("Mandate = " + str(m * 100) + "%")
    plt.yscale("log")
    plt.title("CDF of crossover iterations (log scale)")
    plt.legend()
    plt.xlabel("Iteration")
    plt.ylabel("Count")
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

        plt.hist(convergences, bins=100,  cumulative=True,  histtype='step', label=str(m * 100) + "% mandate", density=True)

    # plt.ylim(0,10000)
    plt.title("CDF of convergence iterations (log scale)")
    plt.yscale("log")
    plt.xlabel("Iteration")
    plt.ylabel("Count")
    plt.legend()
    plt.tight_layout()
    plt.show()  

def loss_ratio_hist(df):
    
    # fig, a =  plt.subplots(2,5,sharey=True,sharex=True)

    mandates = sorted(df['SEC_INVESTMENT'].unique())
    for i,m in enumerate(mandates):
        if (m == 1):
            continue

        print(m)
        mands = df.loc[df['SEC_INVESTMENT'] == m]

        d_inits = mands['d_init'].to_numpy() / (1 - float(m))
        print(d_inits)
        d_ends = mands['d_end'].to_numpy()

        convergences = mands['final_iter'].to_numpy()
        convergences = convergences - 50


        ratios = []
        for start,finish in zip(d_inits, d_ends):
            ratios.append((start - finish)/start)
        
        # row = i//5
        # col = i%5
        # print(row, col)
        # a[0, 0].hist(np.zeros(100), bins=100)
        plt.hist(ratios, bins=np.linspace(0,1,100),  cumulative=True,  histtype='step', label=str(m * 100) + "% mandate", density=True)
        # a[row, col].set_title("Mandate = " + str(m * 100) + "%")
        # a[row, col].set_ylim(bottom=1)
        # a[row, col].yscale("log")
    # plt.ylim(0, 1*10**6)

    # plt.yscale("log")
    # plt.ylim(0,10000)
    plt.title("CDF of percent of assets lost (linear scale)")
    plt.xlabel("% of assets lost")
    plt.ylabel("Count")
    # plt.tight_layout()
    plt.legend()
    plt.show()  

def make_plots(df):
    # print(colored('  [+] Making crossover histogram', 'green'))
    # crossover_hist(df)
    # print(colored('  [+] Making convergence histogram', 'green'))
    # converge_hist(df)
    print(colored('  [+] Making loss ratio histogram', 'green'))
    loss_ratio_hist(df)