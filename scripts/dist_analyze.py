#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Jan 13 15:07:22 2022

@author: ry_pi5
"""
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

params = ['PERCENT_EVIL', 'PAYOFF', 'SEC_INVESTMENT', 'SEC_INVESTMENT_CONVERSION_RATE', 'WEALTH_GAP', 'ATTACK_COST_CONVERSION_RATE']
param_formatted = ['ATTACKERS', 'PAYOFF', 'INVESTMENT', 'EFFECTIVENESS', 'INEQUALITY', 'SUCCESS']
param_legend = ['ATTACKERS', 'INEQUALITY', 'SUCCESS', 'INVESTMENT', 'EFFECTIVENESS','PAYOFF']
si_labels = np.arange(0, 1.1, 0.1).tolist()
si_nums = [round(num,1) for num in si_labels]

def main():
    path = '../data/df_no50.csv'
    dframe = pd.read_csv(path, index_col=False, header=0)
    num_bins = 10
    num_rows = 2
    num_cols = 3
    i = 0
    
    for param in params:
        i += 1
        plt.subplot(num_rows, num_cols, i)
        plt.title(param_formatted[params.index(param)] + " distribution")
        plt.xlabel("Param val")
        plt.ylabel("# of simulations")
        plt.hist(dframe[param], num_bins, facecolor='blue', alpha=0.5, edgecolor='black')
        plt.show()
        
        valcount = dframe[param].value_counts()
        print(param + " : " + str(valcount))
        expected_val = 0
        for pair in valcount.items():
            expected_val += pair[0] * pair[1]
        expected_val = expected_val / valcount.sum()
        print(param + " expected val: " + str(expected_val) )
    
    plt.figure()
    plt.title("Useful Monte-Carlo simulation parameter distributions")
    plt.xlabel("Parameter values: [0, 1]")
    plt.xticks(si_nums)
    plt.ylabel("Number of simulations")
    plt.grid()
    #plt.set_ylim(ymin=0)
    #plt.set_xlim(xmin=0)
    
    for param in params:
        xvals = []
        yvals = []
        param_valcount = dframe[param].value_counts()
        for pair in param_valcount.items():
            xvals.append(pair[0])
            yvals.append(pair[1])
        plt.plot(xvals, yvals)
        plt.legend(param_legend)
    plt.savefig('../figures/hist_mc.pdf')
            
if __name__ == "__main__":
    main()