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
    ProbDefenseSuccess = np.random.normal(0.388, 0.062)
    if ProbDefenseSuccess < 0:
        print("Whoa! Drawing this number is more unlikely than dying in a plane crash!")
        ProbDefenseSuccess = 0
    elif ProbDefenseSuccess > 1:
        print("Wow! Drawing this number is more unlikely than winning the lottery!")
        ProbDefenseSuccess = 1

    ProbOfAttackSuccess = 1 - ProbDefenseSuccess


    costToAttack = assets * cfg.params['ATTACK_COST_CONVERSION_RATE'] # TODO discuss later if we want to change this derivation

    return assets, ProbOfAttackSuccess, costToAttack

def create_red_agent():
    randwealth = [random.randint(0,9999), random.randint(10000,99999), random.randint(100000,999999), random.randint(1000000,9999999)]        

    assets = choice(randwealth, 1, p=[0.55, 0.33, 0.11, 0.01])[0] * cfg.params['WEALTH_GAP']

    return assets

def init_game():

    Defenders = []
    Attackers = []
    
    for _ in range(BLUETEAM_SIZE):
        assets, ProbOfAttackSuccess, costToAttack = create_blue_agent()
        Defenders.append(Defender(assets, ProbOfAttackSuccess, costToAttack))
    
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

    effective_loot = Defender.assets * cfg.params['PAYOFF']
    cost_of_attack = Defender.costToAttack
    expected_earnings = effective_loot * Defender.ProbOfAttackSuccess

    if (expected_earnings > cost_of_attack) and (cost_of_attack < Attacker.assets):
        AttackerWins = np.random.uniform(0,1) < Defender.ProbOfAttackSuccess
        if (AttackerWins):
            Defender.lose(effective_loot)
            Attacker.win(effective_loot, cost_of_attack)
        else:
            Attacker.lose(cost_of_attack)

        # The attacker might get caught
        if (np.random.uniform(0,1) < cfg.params['CHANCE_OF_GETTING_CAUGHT']):
            if (AttackerWins):
                recoup_amount = effective_loot if Attacker.assets > effective_loot else Attacker.assets
                Defender.recoup(recoup_amount) # Defender recoups amount that was lost
                Attacker.lose(recoup_amount)

            Attacker.lose(Attacker.assets)           # Remaining assets are seized by the government
            

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
        for _ in range(cfg.game_settings['SIM_ITERS'] - final_iter - 1):
            d_iters.append(0)
            a_iters.append(a_iters[-1])

    if (a_iters[-1] == 0):
        for _ in range(cfg.game_settings['SIM_ITERS'] - final_iter - 1):
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
                    d.assets -= d.assets * cfg.params['SEC_INVESTMENT']
                    d.costToAttack += (d.assets * cfg.params['SEC_INVESTMENT']) * cfg.params['SEC_INVESTMENT_CONVERSION_RATE']
            
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
