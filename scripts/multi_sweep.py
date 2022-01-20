#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Jan 20 17:42:04 2022

@author: ry_pi5
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

base_path = '../data/filtered_'
sweep_vars = ['PAYOFF', 'ACCR', 'SICR', 'SEC_INVESTMENT', 'PERCENT_EVIL', 'WEALTH_GAP']
param_names = ['PAYOFF', 'ATTACK_COST_CONVERSION_RATE', 'SEC_INVESTMENT_CONVERSION_RATE', 'SEC_INVESTMENT', 'PERCENT_EVIL', 'WEALTH_GAP']

legend_labels = np.arange(0, 1.1, 0.1).tolist()
rounded_labels = [str(round(num, 2)) for num in legend_labels]


def main():
    for sweep_var in sweep_vars:
        plt.figure()
        for num in range(0, 11):
            sweep_graph = []
            dframe = pd.read_csv(base_path + sweep_var + "_" + str(float(num/10)) + '.csv', index_col=False, header=0)
            i=0
            for payoff_val in dframe[param_names[sweep_vars.index(sweep_var)]]:
                #make relative
                sweep_graph.append((dframe['d_init'][i] - dframe['d_end'][i]) / (dframe['d_init'][i]))
                i+=1
                
            print(sweep_graph)
            plt.legend(rounded_labels, title="SEC_INV")
            plt.plot(sweep_graph)
            plt.title("sweep of: " + sweep_var)
            plt.xlabel("value of " + sweep_var +  " * 10")
            plt.ylabel("relative $ loss by defenders")
            plt.grid()
    
    
                           
if __name__ == "__main__":
    main()
    