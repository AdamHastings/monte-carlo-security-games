#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Nov 10 15:29:26 2021
"""

# params_ranges = dict(
#     PERCENT_EVIL = np.linspace(0.3, 0.4, 2),
#     PAYOFF = np.linspace(0.3, 0.4, 2),
#     WEALTH_GAP = np.linspace(0.3, 0.4, 2),
#     SEC_INVESTMENT_CONVERSION_RATE = np.linspace(0.3, 0.4, 2),
#     ATTACK_COST_CONVERSION_RATE = np.linspace(0.3, 0.4, 2),
#     CHANCE_OF_GETTING_CAUGHT = np.linspace(0.3, 0.4, 2),
#     SEC_INVESTMENT = np.linspace(0.3, 0.4, 2)
# )

import os
import threading
import numpy as np

CONFIG_FILE = 'cfg1'

PARALLEL_INDEX = 6 #zero index based on parameter list above
PARALLEL_RANGE = np.linspace(0, 1, 2)

def spawn_game(entry):
    os.system('python3 game.py ' + CONFIG_FILE + ' ' + entry + ' ' + str(PARALLEL_INDEX))
    
def main():

    for entry in PARALLEL_RANGE:
        print('python3 game.py ' + CONFIG_FILE + ' ' + str(entry) + ' ' + str(PARALLEL_INDEX))  
        threading.Thread(target=spawn_game, args=(str(entry),)).start()

if __name__== "__main__":
  main()


