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
human_readable = ['attacker bounty', '(initial assets/cost of attack)', '(security mandate percentage/cost of attack)', 'security mandate percentage', 'red team size', 'attacker-defender wealth gap']

si_labels = np.arange(0, 1.1, 0.1).tolist()
si_nums = [round(num,1) for num in si_labels]
rounded_labels = [(str(int(num * 100)) + "%") for num in si_nums]

si_xvals = [round(num, 2) for num in si_labels]

norm_vals = np.arange(0.1, 1.1, 0.1).tolist()
xvals = [round(num,2) for num in norm_vals]


def main():
    for sweep_var in sweep_vars:
        plt.figure()
        for num in range(0, 11):
            sweep_graph = []
            #another array to store total percent loss relative to original assets
            total_graph = []
                
            dframe = pd.read_csv(base_path + sweep_var + "_" + str(float(num/10)) + '.csv', index_col=False, header=0)
            if sweep_var == 'SEC_INVESTMENT':
                dframe = pd.read_csv(base_path + sweep_var  + '.csv', index_col=False, header=0)
            
            i=0
            for payoff_val in dframe[param_names[sweep_vars.index(sweep_var)]]:
                #make relative
                sweep_graph.append((dframe['d_init'][i] - dframe['d_end'][i]) / (dframe['d_init'][i]))
                #total_graph.append((dframe['d_init'][i] - dframe['d_end'][i] +))
                i+=1
            
            print ("VALUES FOR " + sweep_var)
            print(sweep_graph)

            if sweep_var != 'SEC_INVESTMENT':
                plt.legend(rounded_labels, title="Mandate")
                plt.plot(xvals, sweep_graph)
            else:
                plt.plot(si_xvals, sweep_graph)
                
            plt.title("Total relative defender losses vs. " + human_readable[sweep_vars.index(sweep_var)])
            plt.xlabel(human_readable[sweep_vars.index(sweep_var)] + ": (0, 1]")
            plt.xticks(si_nums)
            plt.ylabel("Relative $ loss by defenders (%)")
            plt.grid()
        plt.savefig('../figures/' + sweep_var + '.pdf')
    
    
                           
if __name__ == "__main__":
    main()
    