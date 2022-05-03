#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Jan 20 17:42:04 2022

@author: ry_pi5
"""

import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as mtick
import numpy as np
plt.rcParams["font.family"] = "Times New Roman"
plt.rcParams["font.size"] = 9

base_path = '../data/filtered_'
sweep_vars = ['PAYOFF', 'ACCR', 'SICR']#, 'SEC_INVESTMENT', 'PERCENT_EVIL', 'WEALTH_GAP']
param_names = ['PAYOFF', 'ATTACK_COST_CONVERSION_RATE', 'SEC_INVESTMENT_CONVERSION_RATE', 'SEC_INVESTMENT', 'PERCENT_EVIL', 'WEALTH_GAP']
human_readable = ['PAYOFF', 'success', 'EFFICIENCY', 'investment', 'number of attackers', 'inequality']

si_labels = np.arange(0, 1.1, 0.1).tolist()
si_nums = [round(num,1) for num in si_labels]
# rounded_labels = [(str(int(float(num) * 100)) + "%") for num in si_nums]

si_xvals = [round(num, 2) for num in si_labels]

norm_vals = np.arange(0.1, 1.1, 0.1).tolist()
xvals = [round(num,2) for num in norm_vals]
print(xvals)


def main():
    for sweep_var in sweep_vars:

        plt.clf()
        fig = plt.figure(figsize=(4,3))
        ax = fig.add_subplot(1,1,1)
        

        for num in range(0, 10):
            sweep_graph = []
            #another array to store total percent loss relative to original assets
            total_graph = []
                
            dframe = pd.read_csv(base_path + sweep_var + "_" + str(si_nums[num]) + '.csv', index_col=False, header=0)
            if sweep_var == 'SEC_INVESTMENT':
                dframe = pd.read_csv(base_path + sweep_var  + '.csv', index_col=False, header=0)
            
            i=0
            for payoff_val in dframe[param_names[sweep_vars.index(sweep_var)]]:
                #make relative
                #sweep_graph.append((dframe['d_init'][i] - dframe['d_end'][i]) / (dframe['d_init'][i]))
                if dframe['d_init'][i] != 0:
                    sweep_graph.append(((dframe['d_init'][i]/(1 - dframe['SEC_INVESTMENT'][i])) - dframe['d_end'][i] ) / (dframe['d_init'][i]/(1 - dframe['SEC_INVESTMENT'][i])))
                else:
                    sweep_graph.append(1)
                i+=1
            
            sweep_graph = np.array(sweep_graph) * 100

            ax.plot(xvals, sweep_graph, label=str(num*10) + "%")
            ax.yaxis.set_major_formatter(mtick.PercentFormatter())

                
            
            handles, labels = ax.get_legend_handles_labels()
            # print(handles)
            # print(labels)
            # print(rounded_labels)
            # print(list(reversed(rounded_labels)))
            handles, labels = ax.get_legend_handles_labels()

            plt.grid(True, which='both')
            # plt.title("Total inclusive relative defender losses vs. " + human_readable[sweep_vars.index(sweep_var)])

        print(human_readable[sweep_vars.index(sweep_var)])

        plt.legend(reversed(handles), reversed(labels), title="MANDATE:", bbox_to_anchor=(1.3,1), loc="upper right", fancybox=True, shadow=True, ncol=1)
        plt.xlabel(human_readable[sweep_vars.index(sweep_var)] + ": (0, 1]")
        plt.xticks(si_nums)
        plt.ylabel("Relative token loss by defenders (%)")
        plt.ylim(0,100)
        plt.tight_layout()
        plt.savefig('../figures/' + sweep_var +  '_inclusive.pdf')
    
    
                           
if __name__ == "__main__":
    main()
    