
from fileinput import filename
from datetime import datetime
from Agent import *
from Game import *
from multiprocessing import Pool
import itertools
import numpy as np
import importlib
import sys
import random
from numpy.random import choice
from copy import deepcopy
import math
import os.path

def run_games(ATTACKERS, PAYOFF, INEQUALITY, EFFICIENCY, EFFORT, CAUGHT, CLAIMS, PREMIUM, TAX, MANDATE):

    global cfg
    global gDefenders 
    global gAttackers
    global gInsurer
    global gGovernment

    params = {}
    params["MANDATE"] = MANDATE
    params["ATTACKERS"] = ATTACKERS
    params["INEQUALITY"] = INEQUALITY
    params["PREMIUM"] = PREMIUM
    params["EFFICIENCY"] = EFFICIENCY
    params["EFFORT"] = EFFORT
    
    params["PAYOFF"] = PAYOFF
    params["CAUGHT"] = CAUGHT
    params["CLAIMS"] = CLAIMS 
    params["TAX"] = TAX
    


    for i in range(cfg.game_settings['NUM_GAMES']):
        
        # Create Agents here
        Defenders = deepcopy(gDefenders)
        Insurer = deepcopy(gInsurer)
        Government = deepcopy(gGovernment)
        Attackers = deepcopy(gAttackers[:int(cfg.game_settings["BLUE_PLAYERS"] * params["ATTACKERS"])])
        
        for a in Attackers:
            a.assets *= params['INEQUALITY']
            assert a.assets > 0, str(params)

        for d in Defenders:
            investment = d.assets * params["MANDATE"]

            # Selfish = Insurance. Provides coverage without changing the landscape (just gives more money to attackers in the long run!)
            # Selfless = Private security spending + taxes. Actually can improve the situation for *other* defenders as well.

            selfish_investment = investment * params["PREMIUM"]
            d.lose(selfish_investment)
            Insurer.gain(selfish_investment)

            selfless_investment = investment - selfish_investment
            taxes = selfless_investment * params["TAX"]
            d.lose(taxes)
            Government.gain(taxes)

            personal_security_investment = selfless_investment - taxes

            assert (investment - (selfish_investment + taxes + personal_security_investment)) + 1 >= 0

            d.lose(personal_security_investment)

            d.costToAttack = d.assets * params["EFFORT"]
            d.costToAttack += (personal_security_investment * params["EFFICIENCY"])

            assert d.assets >=0, str(params)

        # Create a Game object to hold game parameters
        g = Game(game_settings=cfg.game_settings, params=params, Attackers=Attackers, Defenders=Defenders, Insurer=Insurer, Government=Government)
        
        g.run_iterations()
        
        log = open(cfg.LOGFILE, 'a') 
        log.write(str(g))
        log.close()


'''
    Make sure you don't accidentally overwrite an existing logfile
'''
def init_logs(cfg):
    if os.path.exists(cfg.LOGFILE):
        response = input("\nThis file already exists: " + cfg.LOGFILE + "\nDo you want to replace it? Y/n\n >> ")
        if response.lower() != "y":
            print("\nOK, this program will not overwrite " + cfg.LOGFILE + ".\nThis program will now exit.\n")
            sys.exit(0)
        else:
            print("\n")
    print("Writing to", cfg.LOGFILE)

    log = open(cfg.LOGFILE, 'w')  # write mode. Overwrite existing file
    header = ""
    for k in sorted(cfg.params_ranges.keys()):
        header += k[:-6] + "," # trim off the "_range" of the cfg param names
    
    # Make sure this lines up with what Game's __str__ method returns
    header += "d_init,d_end,a_init,a_end,i_init,i_end,g_init,g_end,attacks_attempted,attacks_succeeded,amount_stolen,attacker_expenditures,crossovers,insurer_tod,paid_claims,final_iter,outcome\n"
    log.write(header)
    log.close()


'''
    Initialize worker processes with some global variables.
    This is so that we don't have to do create 
    Called once per core.
'''
def init_worker(cfg_in):
    random.seed(3) # Seed random so that Agents are the same across multiple cores

    global cfg
    cfg = cfg_in

    global gDefenders 
    global gAttackers
    global gInsurer
    global gGovernment

    gDefenders = []    
    for _ in range(cfg.game_settings['BLUE_PLAYERS']):
        gDefenders.append(Defender())
    
    gAttackers = []
    for _ in range(cfg.game_settings['BLUE_PLAYERS']):
        gAttackers.append(Attacker())

    gInsurer = Insurer()
    gGovernment = Government()


def main():
    try:
        cfg = importlib.import_module("configs." + sys.argv[1])
        if len(sys.argv) != 2:
            raise Exception("\nERROR: Incorrect number of args!")
    except Exception as e:
        print(e)
        print("\nExample of how to run config test_cfg1")
        print("\n     $ python3 game.py test_cfg1\n\n")
        sys.exit(0)

    init_logs(cfg)

    start_time = datetime.now()

    inputs = list(itertools.product(cfg.params_ranges["ATTACKERS_range"],
                                        cfg.params_ranges["PAYOFF_range"],
                                        cfg.params_ranges["INEQUALITY_range"],
                                        cfg.params_ranges["EFFICIENCY_range"],
                                        cfg.params_ranges["EFFORT_range"],
                                        cfg.params_ranges["CAUGHT_range"],
                                        cfg.params_ranges["CLAIMS_range"],
                                        cfg.params_ranges["PREMIUM_range"],
                                        cfg.params_ranges["TAX_range"],
                                        cfg.params_ranges["MANDATE_range"]
                                    ))

    print("Starting", len(inputs), "games at", start_time.strftime("%H:%M:%S"))
    with Pool(initializer=init_worker, initargs=(cfg,)) as p:
        p.starmap(run_games, inputs)

    end_time = datetime.now()
    print("\nFinished! at", end_time.strftime("%H:%M:%S"))
    
    elapsed_time = end_time - start_time
    # print(elapsed_time)
    # a = elapsed_time.strftime("%H:%M:%S")
    print("Runtime: ", elapsed_time, "\n")
    
if __name__== "__main__":
  main()
