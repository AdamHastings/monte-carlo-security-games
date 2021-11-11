#!/usr/bin/env python3
# -*- coding: utf-8 -*-


from threading import Thread
import numpy as np
from colorama import Fore, Style
import os, sys
import importlib
import copy
from simulate import run_games


PARALLEL_INDEX = 'SEC_INVESTMENT'
CONFIG_FILE = 'cfg1'

def get_parallel_index():
    return PARALLEL_INDEX

try:
    cfg = importlib.import_module("configs." + sys.argv[1])       
except:
    print(Fore.RED + "ERROR: Config file not found, or maybe another error! :) ")
    sys.exit(0)


def spawn_game(entry):
    os.system('python3 game.py ' + CONFIG_FILE + ' ' + entry + ' ' + str(PARALLEL_INDEX))

def launch(p_cfg):
    run_games(p_cfg)

def create_logs(parallel_linspace):

    for p in parallel_linspace:
        
        filename = 'logs/stats_' + PARALLEL_INDEX + '_' + str(p) + ".csv"
        statsfile = open(filename, 'w')  # write mode
        
        for k,v in cfg.params_ranges.items():
            statsfile.write(str(k) + ',')

        statsfile.write('d_init,d_end,a_init,a_end,final_iter\n')
        statsfile.close()

    
def main():


    parallel_linspace = cfg.params_ranges[PARALLEL_INDEX]
    cfg.params_ranges[PARALLEL_INDEX] = np.linspace(0,0,1)

    create_logs(parallel_linspace)


    for PERCENT_EVIL in cfg.params_ranges['PERCENT_EVIL']:
        for PAYOFF in cfg.params_ranges['PERCENT_EVIL']:
            for WEALTH_GAP in cfg.params_ranges['WEALTH_GAP']:
                for SEC_INVESTMENT_CONVERSION_RATE in cfg.params_ranges['SEC_INVESTMENT_CONVERSION_RATE']:
                    for ATTACK_COST_CONVERSION_RATE in cfg.params_ranges['ATTACK_COST_CONVERSION_RATE']:
                        for CHANCE_OF_GETTING_CAUGHT in cfg.params_ranges['CHANCE_OF_GETTING_CAUGHT']:
                            for SEC_INVESTMENT in cfg.params_ranges['SEC_INVESTMENT']:

                                params = {}
                                params['PERCENT_EVIL'] = PERCENT_EVIL
                                params['PAYOFF'] = PAYOFF
                                params['WEALTH_GAP'] = WEALTH_GAP
                                params['SEC_INVESTMENT_CONVERSION_RATE'] = SEC_INVESTMENT_CONVERSION_RATE
                                params['ATTACK_COST_CONVERSION_RATE'] = ATTACK_COST_CONVERSION_RATE
                                params['CHANCE_OF_GETTING_CAUGHT'] = CHANCE_OF_GETTING_CAUGHT
                                params['SEC_INVESTMENT'] = SEC_INVESTMENT

                                threads = []

                                # The param that will be parallelized:
                                for p in parallel_linspace:                                    
                                    p_params = copy.deepcopy(params)

                                    p_params[PARALLEL_INDEX] = p

                                    t = Thread(target=run_games, args=(p_params, PARALLEL_INDEX, ))
                                    threads.append(t)
                                
                                for t in threads:
                                    t.start()

                                for t in threads:
                                    t.join()


if __name__== "__main__":
  main()


