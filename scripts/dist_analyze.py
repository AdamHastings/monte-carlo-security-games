#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Jan 13 15:07:22 2022

@author: ry_pi5
"""
import pandas as pd
import matplotlib.pyplot as plt

params = ['PERCENT_EVIL', 'PAYOFF', 'SEC_INVESTMENT', 'SEC_INVESTMENT_CONVERSION_RATE', 'WEALTH_GAP', 'ATTACK_COST_CONVERSION_RATE']
param_formatted = ['PERCENT_EVIL', 'PAYOFF', 'SEC_INVESTMENT', 'SICR', 'WEALTH_GAP', 'ACCR']

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
        #print(param + " : " + str(valcount))
        expected_val = 0
        for pair in valcount.items():
            expected_val += pair[0] * pair[1]
        expected_val = expected_val / valcount.sum()
        print(param + " expected val: " + str(expected_val) )
            

    

    
if __name__ == "__main__":
    main()