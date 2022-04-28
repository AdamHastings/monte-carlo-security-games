#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as mtick
from matplotlib.lines import Line2D
from termcolor import colored
from matplotlib.ticker import AutoMinorLocator, FormatStrFormatter
plt.rcParams["font.family"] = "Times New Roman"
plt.rcParams["font.size"] = 9




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
    plt.legend(loc="upper right", title="Mandated security spending", fancybox=True, shadow=True, ncol=1)
    plt.xlabel("Iteration")
    plt.ylabel("Percent of simulations")
    # plt.xlabel("Crossover iteration (far-left red bar means no crossover)")
    # plt.ylabel("Count")
    # plt.show()  

def total_loot_hist(df):
    # fig,a =  plt.subplots(2,5,sharey=True, sharex=True)

    plt.clf()
    fig = plt.figure(figsize=(4,3))
    a_win = fig.add_subplot(1,1,1)

    #neither_win = fig.add_subplot(1,3,3)

    mandates = sorted(df['SEC_INVESTMENT'].unique())
    for i,m in enumerate(mandates):
        #if (m == 1 or m == 0.9):
        if (m > 0.4):
            continue

        mands = df.loc[df['SEC_INVESTMENT'] == m]
        print(str(mands.to_numpy().size) + " is total size for this mandate")
        
        #Probe convergence conditions
        attacker_wins = mands.loc[mands['d_end'] == 0]
        defender_wins = mands.loc[mands['a_end'] == 0]   
        neither_wins = mands.loc[mands['d_end'] != 0]

        
        #convergences = mands['final_iter'].to_numpy()
        
        a_convergences = attacker_wins['final_iter'].to_numpy()
        d_convergences = defender_wins['final_iter'].to_numpy()
        n_convergences = neither_wins['final_iter']
        print("Neither wins: " + str(n_convergences.to_numpy().size) + " scenarios")
        
        print("Defenders totally looted in " + str(a_convergences.size) + " scenarios")
        print("Attackers completely looted in " + str(d_convergences.size) + " scenarios")

        print("Total: " + str(a_convergences.size + d_convergences.size + n_convergences.size) + " simulations")
        
        #Commenting this out, I think we need to show the 50 iter min -- ryan
        #convergences = convergences - 50

        N = a_convergences.size
                
        #Good for CDF, but we are changing to PDF
        #X1 = np.sort(a_convergences)
        #F1 = np.array(range(N))/float(N) * 100
        
        binsize = 50
        round_vals = [int(val/binsize) * binsize for val in a_convergences]
        (vals, counts) = np.unique(round_vals, return_counts=True)
        X1 = vals
        F1 = counts
        
        # X2 = np.sort(d_convergences)
        # F2 = np.array(range(N))/float(N) * 100
        
        # X3 = np.sort(n_convergences)
        # F3 = np.array(range(N))/float(N) * 100

        a_win.plot(X1, F1, label=str(int(m * 100)) + "%", linewidth=2)
        #d_win.step(X2, F2, label=str(int(m * 100)) + "%")
        #neither_win.step(X3, F3, label=str(int(m * 100)) + "%")

    plt.ylim(0, 375)
    # plt.title("PDF of simulation iterations when attackers win")
    #ax.yaxis.set_major_formatter(mtick.PercentFormatter(decimals=0))
    # plt.yscale("log")
    plt.xlim(left=0, right=600)
    plt.minorticks_on()
    plt.grid(True, which='both')
    plt.xlabel("Duration of simulation (iterations)")
    plt.ylabel("Number of simulations")
    plt.legend(loc="upper right", title="Mandate:", fancybox=True, shadow=True, ncol=1)
    plt.tight_layout()
    # plt.show() 
    plt.savefig("../figures/total_loot.pdf")
    
def rate_hist(df):
    # fig,a =  plt.subplots(2,5,sharey=True, sharex=True)

    plt.clf()
    fig = plt.figure(figsize=(4, 3))
    ax = fig.add_subplot(1,1,1)

    mandates = sorted(df['SEC_INVESTMENT'].unique())
    for i,m in enumerate(mandates):
        if (m > 0.7):
            continue

        mands = df.loc[df['SEC_INVESTMENT'] == m]
        print(str(mands.to_numpy().size) + " is total size for this mandate")
        
        #Probe convergence conditions
        attacker_wins = mands.loc[mands['d_end'] == 0]
        defender_wins = mands.loc[mands['a_end'] == 0]   
        neither_wins = mands.loc[mands['d_end'] != 0]
        
        a_convergences = attacker_wins['final_iter'].to_numpy()
        d_convergences = defender_wins['final_iter'].to_numpy()
        n_convergences = neither_wins['final_iter']
        
        neither_wins['rate'] = (neither_wins['d_init'] - neither_wins['d_end']) / (neither_wins['final_iter'])
        rates = neither_wins['rate'].to_numpy()

        N = rates.size
        
        X1 = np.sort(rates)
        F1 = np.array(range(N))/float(N) * 100
        
        ax.step(X1, F1, label=str(int(m * 100)) + "%", linewidth=2)


    plt.ylim(60, 102)
    # plt.title("CDF of loss rates for simulations that neither party wins")
    #ax.yaxis.set_major_formatter(mtick.PercentFormatter(decimals=0))
    # plt.yscale("log")
    plt.xlim(left=0, right=1.7e6)
    plt.minorticks_on()
    plt.grid(True, which='both')
    plt.xlabel("Loss rate ($ stolen by attackers / simulation iterations) ")
    plt.ylabel("Percent of simulations")
    plt.legend(loc="lower right", title="Mandate:", fancybox=True, shadow=True, ncol=1)
    plt.tight_layout()
    # plt.show() 
    plt.savefig("../figures/loss_rate.pdf")


def loss_ratio_hist(df):
    
    # fig, a =  plt.subplots(2,5,sharey=True,sharex=True)

    plt.clf()
    fig = plt.figure(figsize=(4,3))
    ax = fig.add_subplot(1,1,1)

    mandates = sorted(df['SEC_INVESTMENT'].unique())

    custom_lines = []
    for i,m in enumerate(mandates):
        if (m > 0.7):
            continue

        print("*********---------------")

        # print(m)
        mands = df.loc[df['SEC_INVESTMENT'] == m]

        d_inits = mands['d_init'].to_numpy() / (1 - float(m))
        # print(d_inits)
        d_ends = mands['d_end'].to_numpy()

        convergences = mands['final_iter'].to_numpy()
        #I think we should show min is 50 -- Ryan
        #convergences = convergences - 50


        ratios = []
        for start,finish in zip(d_inits, d_ends):
            ratios.append(((start - finish)/start) * 100)
        
        N = len(ratios)
        print("N = " + str(N))
        X2 = np.sort(ratios)
        F2 = np.array(range(N))/float(N) * 100
        print(X2[0], X2[-1])
        
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
        ax.step(X2, F2, label=str(int(m * 100)) + "%")

        # plt.title("CDF of percent decrease in assets")
        plt.xlabel("Percent decrease in assets (mandated spending + losses from attacks)")
        plt.ylabel("Percent of simulations")
        plt.xlim(0,105)
        plt.ylim(60,100)
        ax.xaxis.set_major_formatter(mtick.PercentFormatter(decimals=0))
        ax.yaxis.set_major_formatter(mtick.PercentFormatter(decimals=0))
        # plt.xaxis.set_minor_locator(AutoMinorLocator())
        # plt.xaxis.set_minor_formatter(FormatStrFormatter("%.3f"))
        print(mandates)
        plt.minorticks_on()
        plt.grid(True, which='both')
        plt.legend(loc="lower right", title="Mandate:", fancybox=True, shadow=True, ncol=1)
        # plt.tight_layout()
        # plt.legend(loc="lower right")
        # plt.show() 
    plt.tight_layout() 
    plt.savefig("../figures/cdf_1.pdf")

    plt.xlim(80,102)
    plt.ylim(95,100)
    plt.title("CDF of percent decrease in assets (closeup)")
    plt.savefig("../figures/closeup.pdf")

def make_plots(df):
    print(colored('  [+] Making crossover histogram', 'green'))
    crossover_hist(df)
    print(colored('  [+] Making total loot histogram', 'green'))
    total_loot_hist(df)
    print(colored('  [+] Making rate of loss histogram', 'green'))
    rate_hist(df)
    print(colored('  [+] Making CDFs', 'green'))
    loss_ratio_hist(df)
    


