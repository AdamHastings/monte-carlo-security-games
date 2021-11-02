from Agent import Defender, Attacker
from scipy.stats import skewnorm
from sklearn.preprocessing import normalize
import matplotlib.pyplot as plt
import numpy as np
import importlib
import sys
import random
import colorama
from colorama import Fore, Style
from numpy.random import choice
from copy import deepcopy
import math


blue_dist=None
red_dist=None
linestyles = ['-', '--', '-.', ':']

try:
    cfg = importlib.import_module("configs." + sys.argv[1])
except:
    print(Fore.RED + "ERROR: Config file not found")
    sys.exit(0)

BLUETEAM_SIZE = int(cfg.game_settings['BLUE_PLAYERS'])
REDTEAM_SIZE = int(cfg.game_settings['BLUE_PLAYERS'] * cfg.params['PERCENT_EVIL'])

def create_blue_agent():
    randwealth = [random.randint(0,9999), random.randint(10000,99999), random.randint(100000,999999), random.randint(1000000,9999999)]
    
    assets = choice(randwealth, 1, p=[0.55, 0.33, 0.11, 0.01])[0]
    likelihood_of_attack = np.random.normal(0.388, 0.062)
    if likelihood_of_attack < 0:
        print("Whoa! Drawing this number is more unlikely than dying in a plane crash!")
        likelihood_of_attack = 0
    elif likelihood_of_attack > 1:
        print("Wow! Drawing this number is more unlikely than winning the lottery!")
        likelihood_of_attack = 1

    return assets, likelihood_of_attack

def create_red_agent():
    randwealth = [random.randint(0,9999), random.randint(10000,99999), random.randint(100000,999999), random.randint(1000000,9999999)]        

    assets = choice(randwealth, 1, p=[0.55, 0.33, 0.11, 0.01])[0] * cfg.params['WEALTH_GAP']

    return assets

def init_game():

    Defenders = []
    Attackers = []
    
    for _ in range(BLUETEAM_SIZE):
        assets, likelihood_of_attack = create_blue_agent()
        Defenders.append(Defender(assets, likelihood_of_attack))
    
    for _ in range(REDTEAM_SIZE):
        assets = create_red_agent()
        Attackers.append(Attacker(assets))

    return Attackers, Defenders


def compute_utility(Attackers, Defenders, a_iters, d_iters):
    defenders_assets = 0
    for x in Defenders:
        defenders_assets += x.assets
    d_iters.append(defenders_assets)

    attackers_assets = 0
    for x in Attackers:
        attackers_assets += x.assets
    a_iters.append(attackers_assets)

    return defenders_assets, attackers_assets
    

def fight(Defender, Attacker):

    # possible_earnings = Defender.assets * cfg.game['PAYOFF']
    effective_loot = Defender.assets * cfg.params['PAYOFF']
 
    # cost_of_attack = effective_loot * (Defender.skill / Attacker.skill)
    if (np.random.uniform(0,1) < Defender.likelihood_of_attack):
        # if (cost_of_attack < effective_loot): # always true under current conditions
            Defender.lose(effective_loot)
            # Attacker.win(effective_loot, cost_of_attack)
            Attacker.win(effective_loot, 0)
    # else:
    #    Attacker.lose(cost_of_attack)

def prune(Attackers, Defenders):

    Temp = []
    for i in range(len(Defenders)):
        if Defenders[i].get_assets() > 0:
            Temp.append(Defenders[i])

    Defenders = Temp

    Temp = []
    for i in range(len(Attackers)):
        if Attackers[i].get_assets() > 0:
            Temp.append(Attackers[i])

    Attackers = Temp

def run_iterations(Attackers, Defenders):
    crossover = -1
    final_iter = -1

    a_iters = []
    d_iters = []

    for iter_num in range(cfg.game_settings['SIM_ITERS']):

        # print(Defenders)
        random.shuffle(Defenders)
        # random.shuffle(Attackers)

        for i in range(len(Attackers)):
            if (i > len(Defenders)):
                print("more attackers than defenders!")
                break
            fight(Defenders[i], Attackers[i])
        
        prune(Attackers, Defenders)

        # for i in range(len(Defenders)):
        #     Defenders[i].assets += cfg.blue['EARNINGS']
        
        bsum, rsum = compute_utility(Attackers, Defenders, a_iters, d_iters)

        if ((crossover < 0) and rsum > bsum):
            crossover = iter_num
            print("Crossover at " + str(crossover) + " iterations")

        if len(Defenders) == 0:
            final_iter = iter_num
            print("All Defenders dead after " + str(final_iter) + " iterations")
            break
        if len(Attackers) == 0:
            final_iter = iter_num
            print("All Attackers dead after " + str(final_iter) + " iterations")
            break

    if final_iter == -1:
        print("More plunder possible!")
    
    # pad with zeros
    if (d_iters[-1] == 0):
        for _ in range(cfg.game_settings['SIM_ITERS'] - final - 1):
            d_iters.append(0)
            a_iters.append(a_iters[-1])

    if (a_iters[-1] == 0):
        for _ in range(cfg.game_settings['SIM_ITERS'] - final - 1):
            d_iters.append(d_iters[-1])
            a_iters.append(0)

    print("End of iterations")
    return a_iters, d_iters, crossover, final_iter


def run_games():

    fig, (ax0, ax1) = plt.subplots(nrows=1, ncols=2, sharex=True, sharey=True)

    for i in range(cfg.game_settings['NUM_GAMES']):
        print("game " + str(i))
        red, blue = init_game()

        for mandate in [False, True]:
            print("")


            print("Mandate is: " + str(mandate))

            Attackers = deepcopy(red)
            Defenders = deepcopy(blue)

            if (mandate): # apply the mandate
                for d in Defenders:
                    d.assets = d.assets * (1 - cfg.game_settings['SEC_INVESTMENT'])
                    d.likelihood_of_attack = d.likelihood_of_attack * (1 - cfg.game_settings['SEC_INVESTMENT'])
            
            a_iters, d_iters, crossover, final = run_iterations(Attackers, Defenders)
            
            if (mandate):
                ax1.set_title("With Mandate")
                ax1.plot(d_iters, label="Blue Team", linewidth=2, linestyle="-", color="b")
                ax1.plot(a_iters, label="Red Team", linewidth=2, linestyle="-", color="r")
                if (crossover > 0):
                    ax1.axvline(x=crossover)
            else:
                ax0.set_title("Without Mandate")
                ax0.plot(d_iters, label="Blue Team", linewidth=2, linestyle="-", color="b")
                ax0.plot(a_iters, label="Red Team", linewidth=2, linestyle="-", color="r")
                if (crossover > 0):
                    ax0.axvline(x=crossover)


    plt.xlabel("iterations")
    plt.ylabel("total assets")
    plt.legend()
    plt.show()


def main():
    print("Starting games...")
    random.seed(3)
    run_games()

    
  
if __name__== "__main__":
  main()
