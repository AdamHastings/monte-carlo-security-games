#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from termcolor import colored


def crossover_hist(df):
    crossovers = df['crossover'].to_numpy()
    crossovers = np.where(crossovers == -1, -100, crossovers)
    n, bins, patches = plt.hist(crossovers, bins=100)
    patches[0].set_fc('r')
    plt.yscale("log")
    plt.title("Histogram of crossover iterations (log scale)")
    plt.xlabel("Crossover iteration (far-left red bar means no crossover)")
    plt.ylabel("Count")
    # plt.show()  

def converge_hist(df):
    convergences = df['final_iter'].to_numpy()
    convergences = convergences - 50
    plt.hist(convergences, bins=100)
    plt.yscale("log")
    plt.title("Histogram of convergence iterations (log scale)")
    plt.xlabel("Convergence iteration")
    plt.ylabel("Count")
    # plt.show()  

def loss_ratio_hist(df):
    d_inits = df['d_init'].to_numpy()
    d_ends = df['d_end'].to_numpy()

    ratios = []
    for a,b in zip(d_inits, d_ends):
        if (a == 0):
            continue
        ratios.append(float(b)/float(a))
    plt.hist(ratios, bins=100)
    plt.yscale("log")
    plt.title("Histogram of percent of assets lost (log scale)")
    plt.xlabel("% of assets lost")
    plt.ylabel("Count")
    plt.show()  

def make_plots(df):
    # print(colored('  [+] Making crossover histogram', 'green'))
    # crossover_hist(df)
    # print(colored('  [+] Making convergence histogram', 'green'))
    # converge_hist(df)
    print(colored('  [+] Making loss ratio histogram', 'green'))
    loss_ratio_hist(df)