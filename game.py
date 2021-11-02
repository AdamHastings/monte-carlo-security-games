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

BLUETEAM_SIZE = int(cfg.game_settings['TOTAL_PLAYERS'] * (1 - cfg.params['PERCENT_EVIL']))
REDTEAM_SIZE = int(cfg.game_settings['TOTAL_PLAYERS'] * (cfg.params['PERCENT_EVIL']))


def get_agent_params(agent):
    if agent=="blue":
        # skills = random.choice(blue_dist)
        # skills = np.random.normal(0,1)
        # assets = cfg.blue['ASSETS']
        # skills = np.random.lognormal(0,1,1)[0]
        randwealth = [random.randint(0,9999), random.randint(10000,99999), random.randint(100000,999999), random.randint(1000000,9999999)]
        assets = choice(randwealth, 1, p=[0.55, 0.33, 0.11, 0.01])[0]


        skills = 0.25
        # if (Mandate):
        #     assets = assets * (1 - cfg.game_settings['SEC_INVESTMENT'])
        #     skills = skills * (1 + 50 * cfg.game_settings['SEC_INVESTMENT'])
    elif agent=="red":
        # skills = random.choice(red_dist)
        randwealth = [random.randint(0,9999), random.randint(10000,99999), random.randint(100000,999999), random.randint(1000000,9999999)]        
        # skills = np.random.normal(0,1)
        # assets = cfg.red['ASSETS']
        # skills = np.random.lognormal(0,1,1)[0] +1
        assets = choice(randwealth, 1, p=[0.55, 0.33, 0.11, 0.01])[0] * cfg.params['WEALTH_GAP']
        skills = 2.5 


    else:
        sys.exit(0)

    return assets, skills

def init_game(Mandate=False):

    Defenders = []
    Attackers = []

    Defenders.clear()
    Attackers.clear()

    #assets_dist = []
    blue_power = []
    
    for _ in range(BLUETEAM_SIZE):
        assets, skills = get_agent_params("blue")
        blue_power.append(skills * assets)
        #assets_dist.append(assets)
        x = Defender(assets, skills)
        Defenders.append(x)
    


    red_power = []
    for _ in range(REDTEAM_SIZE):
        assets, skills = get_agent_params("red")
        #assets_dist.append(assets)
        red_power.append(skills * assets)
        x = Attacker(assets, skills)
        Attackers.append(x)

    return Attackers, Defenders


def compute_utility(Attackers, Defenders, a_iters, d_iters):
    defenders_assets = 0
    for x in Defenders:
        defenders_assets += x.assets
    # d_iters.append(defenders_assets/len(Defenders))
    d_iters.append(defenders_assets)

    attackers_assets = 0
    for x in Attackers:
        attackers_assets += x.assets
    # a_iters.append(attackers_assets/len(Attackers))
    a_iters.append(attackers_assets)

    return defenders_assets, attackers_assets
    

def fight(Defender, Attacker):

    # possible_earnings = Defender.assets * cfg.game['PAYOFF']
    effective_loot = Defender.assets # * cfg.params['PAYOFF']
    #cost_of_attack = (effective_loot / cfg.params['ROI'])# * (Defender.skill / Attacker.skill)

    # red_fight_level = Attacker.skill * Attacker.assets
    # blue_fight_level = Defender.skill * Defender.assets
    

    if (Attacker.skill > Defender.skill):
        cost_of_attack = effective_loot * (Defender.skill / Attacker.skill)
        if (cost_of_attack < effective_loot): # always true under current conditions
            Defender.lose(effective_loot)
            Attacker.win(effective_loot, cost_of_attack)
    #else:
    #    Attacker.lose(cost_of_attack)

def prune(Attackers, Defenders):

    Temp = []
    for i in range(len(Defenders)):
        #if d.get_assets() <= 0:
        #    Defenders.remove(Defender)
        if Defenders[i].get_assets() > 0:
            Temp.append(Defenders[i])

    Defenders = Temp

    Temp = []
    for i in range(len(Attackers)):
        if Attackers[i].get_assets() > 0:
            Temp.append(Attackers[i])

    Attackers = Temp

def run_iterations(Attackers, Defenders, a_iters, d_iters):
    crossover = False
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

        for i in range(len(Defenders)):
            Defenders[i].assets += cfg.blue['EARNINGS']
        
        bsum, rsum = compute_utility(Attackers, Defenders, a_iters, d_iters)

        if ((not crossover) and rsum > bsum):
            print("Crossover at " + str(iter_num) + " iterations")
            crossover = True

        if len(Defenders) is 0:
            print("All Defenders dead")
            return iter_num
        if len(Attackers) is 0:
            print("All Attackers dead")
            return iter_num


def run_games():

    fig, (ax0, ax1) = plt.subplots(nrows=1, ncols=2, sharex=True, sharey=True)

    for i in range(cfg.game_settings['NUM_GAMES']):
        print("game " + str(i))
        red, blue = init_game()

        for mandate in [False, True]:

            d_iters = []
            a_iters = []

            print("Mandate is: " + str(mandate))

            Attackers = deepcopy(red)
            Defenders = deepcopy(blue)

            if (mandate): # apply the mandate
                for d in Defenders:
                    d.assets = d.assets * (1 - cfg.game_settings['SEC_INVESTMENT'])
                    d.skill = d.skill * (1 + cfg.game_settings['SEC_INVESTMENT'])
            
            final = run_iterations(Attackers, Defenders, a_iters, d_iters)

            if final is not None:
                print("Finished after " + str(final) + " iterations")
            else:
                print("Num iterations reached. More plunder possible!!!")

            # pad with zeros
            if (d_iters[-1] == 0):
                for _ in range(cfg.game_settings['SIM_ITERS'] - final - 1):
                    d_iters.append(0)
                    a_iters.append(a_iters[-1])

            if (a_iters[-1] == 0):
                for _ in range(cfg.game_settings['SIM_ITERS'] - final - 1):
                    d_iters.append(d_iters[-1])
                    a_iters.append(0)

            # row = int(i / root)
            # col = int(i % root)
            if (mandate):
                ax1.set_title("With Mandate")
                ax1.plot(d_iters, label="Blue Team", linewidth=2, linestyle="-", color="b")
                ax1.plot(a_iters, label="Red Team", linewidth=2, linestyle="-", color="r")
            else:
                ax0.set_title("Without Mandate")
                ax0.plot(d_iters, label="Blue Team", linewidth=2, linestyle="-", color="b")
                ax0.plot(a_iters, label="Red Team", linewidth=2, linestyle="-", color="r")

            


    plt.xlabel("iterations")
    plt.ylabel("total assets")
    # plt.ylim(0, 200000)
    plt.legend()
    plt.show()


def main():
    # init_env()
    print("Starting games...")
    random.seed(3)
    run_games()

    
  
if __name__== "__main__":
  main()
