#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Nov 10 15:29:26 2021
"""

import os, sys
import threading
import numpy as np
import importlib

CONFIG_FILE = 'cfg1'

try:
    cfg = importlib.import_module("configs." + CONFIG_FILE)
    PARALLEL_RANGE = cfg.params_ranges[cfg.PARALLELIZED]
        
except Exception as e:
    print(e)
    sys.exit(0)

def spawn_game(entry):
    os.system('python3 game.py ' + CONFIG_FILE + ' ' + entry)
    
def main():

    for entry in PARALLEL_RANGE:
        print('python3 game.py ' + CONFIG_FILE + ' ' + str(entry))  
        threading.Thread(target=spawn_game, args=(str(entry),)).start()

if __name__== "__main__":
  main()


