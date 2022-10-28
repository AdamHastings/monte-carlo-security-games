from Agent import Defender, Attacker
#import matplotlib.pyplot as plt
import numpy as np
import importlib
import sys
import random
#import colorama
#from colorama import Fore, Style
from numpy.random import choice
from copy import deepcopy
import math


blue_dist=None
red_dist=None

param_names = ['PERCENT_EVIL','PAYOFF', 'WEALTH_GAP', 'SEC_INVESTMENT_CONVERSION_RATE', 'ATTACK_COST_CONVERSION_RATE', 'CHANCE_OF_GETTING_CAUGHT', 'SEC_INVESTMENT']

PARALLEL_VAL = -1
ROUND_DIGITS = 2

try:
    cfgfile = sys.argv[1]
    cfg = importlib.import_module("configs." + cfgfile)
    if len(sys.argv) > 1:
        PARALLEL_VAL = float(sys.argv[2])
        cfg.params_ranges[cfg.PARALLELIZED] = [PARALLEL_VAL]
        
except Exception as e:
    print(Fore.RED + "ERROR: Config file not found, or maybe another error! :( ")
    print(e)
    for arg in sys.argv:
        print("ARG: " + arg)
    sys.exit(0)


TOTAL_ASSETS = 0
TOTAL_MANDATE_SPENDING = 0
GOV_ASSETS = 0
ATTACK_SPENDING = 0

def create_blue_agent(ATTACK_COST_CONVERSION_RATE):
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


    costToAttack = assets * ATTACK_COST_CONVERSION_RATE # TODO discuss later if we want to change this derivation

    return assets, ProbOfAttackSuccess, costToAttack


def create_red_agent(WEALTH_GAP):
    randwealth = [random.randint(0,9999), random.randint(10000,99999), random.randint(100000,999999), random.randint(1000000,9999999)]        

    assets = choice(randwealth, 1, p=[0.55, 0.33, 0.11, 0.01])[0] * WEALTH_GAP

    return assets


def init_game(BLUETEAM_SIZE, REDTEAM_SIZE, ATTACK_COST_CONVERSION_RATE, WEALTH_GAP):

    Defenders = []
    Attackers = []
    
    for _ in range(BLUETEAM_SIZE):
        assets, ProbOfAttackSuccess, costToAttack = create_blue_agent(ATTACK_COST_CONVERSION_RATE)
        Defenders.append(Defender(assets, ProbOfAttackSuccess, costToAttack))
    
    for _ in range(REDTEAM_SIZE):
        assets = create_red_agent(WEALTH_GAP)
        Attackers.append(Attacker(assets))

    Insurer = Insurer(assets)

    return Attackers, Defenders, Insurer


def sum_assets(Players):
    total_assets = 0
    for x in Players:
        total_assets += x.assets

    return total_assets


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
    
def fight(Defender, Attacker, PAYOFF, CHANCE_OF_GETTING_CAUGHT):

    effective_loot = Defender.assets * PAYOFF
    cost_of_attack = Defender.costToAttack
    expected_earnings = effective_loot * Defender.ProbOfAttackSuccess

    if (expected_earnings > cost_of_attack) and (cost_of_attack < Attacker.assets):
        AttackerWins = np.random.uniform(0,1) < Defender.ProbOfAttackSuccess
        if (AttackerWins):
            Defender.lose(effective_loot)
            Attacker.win(effective_loot, cost_of_attack)
        else:
            Attacker.lose(cost_of_attack)

        global ATTACK_SPENDING
        ATTACK_SPENDING += cost_of_attack

        # The attacker might get caught
        if (np.random.uniform(0,1) < CHANCE_OF_GETTING_CAUGHT):
            if (AttackerWins):
                recoup_amount = effective_loot if Attacker.assets > effective_loot else Attacker.assets
                Defender.recoup(recoup_amount) # Defender recoups amount that was lost
                Attacker.lose(recoup_amount)

            global GOV_ASSETS
            GOV_ASSETS += Attacker.assets
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

def run_iterations(Attackers, Defenders, PAYOFF, CHANCE_OF_GETTING_CAUGHT):
    crossover = -1
    final_iter = -1

    a_iters = []
    d_iters = []
    stats = []
    stability_reached = False
    stable_count = 0

    for iter_num in range(cfg.game_settings['SIM_ITERS']):

        # print(Defenders)
        random.shuffle(Defenders)
        # random.shuffle(Attackers)

        for i in range(len(Attackers)):
            if (i > len(Defenders)):
                print("more attackers than defenders!")
                break
            fight(Defenders[i], Attackers[i], PAYOFF, CHANCE_OF_GETTING_CAUGHT)
        
        prune(Attackers, Defenders)

        # for i in range(len(Defenders)):
        #     Defenders[i].assets += cfg.blue['EARNINGS']
        
        # bsum, rsum = compute_utility(Attackers, Defenders, a_iters, d_iters)
        a_sum = sum_assets(Attackers)
        d_sum = sum_assets(Defenders)
        
        if len(a_iters) > 0:
            last_a_sum = a_iters[-1]
            last_d_sum = d_iters[-1]
            if ((abs(a_sum - last_a_sum) < cfg.game_settings['EPSILON_DOLLARS']) and (abs(d_sum - last_d_sum) < cfg.game_settings['EPSILON_DOLLARS'])):
                stable_count += 1
                if stable_count >= cfg.game_settings['STABLE_ITERS']:
                    final_iter = iter_num
                    # print("Epsilon threshold of " + str(cfg.game_settings['EPSILON_DOLLARS']) + " reached at " + str(final_iter) + " iterations")
                    stats.append((d_iters[0], d_sum , a_iters[0], a_sum, final_iter, crossover))
                    stability_reached = True
                    break
            else:
                stable_count = 0

        a_iters.append(a_sum)
        d_iters.append(d_sum)

        if ((crossover < 0) and a_sum > d_sum):
            crossover = iter_num
            # print("Crossover at " + str(crossover) + " iterations")

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
     
    if not stability_reached:
        stats.append((d_iters[0], d_sum , a_iters[0], a_sum, final_iter, crossover))
    
    # pad with zeros
    #if (d_iters[-1] == 0):
    #    for _ in range(cfg.game_settings['SIM_ITERS'] - final_iter - 1):
    #        d_iters.append(0)
    #        a_iters.append(a_iters[-1])

    #if (a_iters[-1] == 0):
    #    for _ in range(cfg.game_settings['SIM_ITERS'] - final_iter - 1):
    #        d_iters.append(d_iters[-1])
    #        a_iters.append(0)

    # print("End of iterations")
    return a_iters, d_iters, stats


def run_games(PERCENT_EVIL, PAYOFF, WEALTH_GAP, SEC_INVESTMENT_CONVERSION_RATE, ATTACK_COST_CONVERSION_RATE, CHANCE_OF_GETTING_CAUGHT, SEC_INVESTMENT):

    BLUETEAM_SIZE = int(cfg.game_settings['BLUE_PLAYERS'])
    REDTEAM_SIZE = int(cfg.game_settings['BLUE_PLAYERS'] * PERCENT_EVIL)


    #fig, (ax0, ax1) = plt.subplots(nrows=1, ncols=2, sharex=True, sharey=True)

    for i in range(cfg.game_settings['NUM_GAMES']):
        print("game " + str(i) + ":( " + str(PERCENT_EVIL) + ", " + str(PAYOFF)+ ", " + str(WEALTH_GAP)+ ", " + str(SEC_INVESTMENT_CONVERSION_RATE)+ ", " + str(ATTACK_COST_CONVERSION_RATE)+ ", " + str(CHANCE_OF_GETTING_CAUGHT) + ", " + str(SEC_INVESTMENT) + ")")
        red, blue = init_game(BLUETEAM_SIZE, REDTEAM_SIZE, ATTACK_COST_CONVERSION_RATE, WEALTH_GAP)

        a_sum = sum_assets(red)
        d_sum = sum_assets(blue)

        TOTAL_ASSETS = a_sum + d_sum
        TOTAL_MANDATE_SPENDING = 0

        # print("INITIAL_BLUE_ASSETS: " + "{:.2e}".format(d_sum))
        # print("INITIAL_RED_ASSETS: " + "{:.2e}".format(a_sum))
        # print("TOTAL_ASSETS: " + "{:.2e}".format(TOTAL_ASSETS))

        # print("")
        # print("Mandate is: " + str(SEC_INVESTMENT > 0))

        global GOV_ASSETS
        global ATTACK_SPENDING


        TOTAL_MANDATE_SPENDING = 0
        GOV_ASSETS = 0
        ATTACK_SPENDING = 0

        Attackers = deepcopy(red)
        Defenders = deepcopy(blue)

        #if (mandate): # apply the mandate
        for d in Defenders:
            investment = d.assets * SEC_INVESTMENT
            d.assets -= investment
            TOTAL_MANDATE_SPENDING += investment
            d.costToAttack += (d.assets * SEC_INVESTMENT * SEC_INVESTMENT_CONVERSION_RATE)
        
        a_iters, d_iters, stats = run_iterations(Attackers, Defenders, PAYOFF, CHANCE_OF_GETTING_CAUGHT)
        
        filename = "logs/stats_" + cfg.PARALLELIZED + "_" + str(round(PARALLEL_VAL, ROUND_DIGITS)) + ".csv" 
        statsfile = open(filename, 'a')  # write mode
        
        statsfile.write(str(PERCENT_EVIL) + "," + str(PAYOFF)+ "," + str(WEALTH_GAP)+ "," + str(SEC_INVESTMENT_CONVERSION_RATE)+ "," + str(ATTACK_COST_CONVERSION_RATE)+ "," + str(CHANCE_OF_GETTING_CAUGHT) + "," + str(SEC_INVESTMENT) + ",")

        for stat in stats[0]:
            statsfile.write(str(stat) + ",")
        statsfile.write('\n')
        statsfile.close()

        # print("GAME STATS: ")
        # print(stats)
        
        # plt.title("")
        # plt.plot(d_iters, label="Blue Team", linewidth=2, linestyle="-", color="b")
        # plt.plot(a_iters, label="Red Team", linewidth=2, linestyle="-", color="r")
        # if (crossover > 0):
        #     plt.axvline(x=crossover)

        # plt.axvline(x=final - cfg.game_settings['STABLE_ITERS'], linestyle='--')

        # print("")
        # print("TOTAL_ASSETS: " + "{:.2e}".format(TOTAL_ASSETS))

        a_sum = sum_assets(Attackers)
        d_sum = sum_assets(Defenders)

        # print("FINAL_BLUE_ASSETS: " + "{:.2e}".format(d_sum))
        # print("FINAL_RED_ASSETS: " + "{:.2e}".format(a_sum))

        # print("GOV_ASSETS: " + "{:.2e}".format(GOV_ASSETS))
        # print("ATTACK_SPENDING: " + "{:.2e}".format(ATTACK_SPENDING))
        # print("TOTAL_MANDATE_SPENDING: " + "{:.2e}".format(TOTAL_MANDATE_SPENDING))
        # print("Check: " + str(True if abs(TOTAL_ASSETS - (a_sum + d_sum + GOV_ASSETS + ATTACK_SPENDING + TOTAL_MANDATE_SPENDING)  < 1) else False))

        # plt.xlabel("iterations")
        # plt.ylabel("total assets")
        # plt.legend()
        # plt.show()

def init_logs():

    filename = 'logs/stats_' + cfg.PARALLELIZED + '_' + str(round(PARALLEL_VAL, ROUND_DIGITS)) + ".csv"
    statsfile = open(filename, 'w')  # write mode
    
    for k in param_names:
        statsfile.write(str(k) + ',')

    statsfile.write('d_init,d_end,a_init,a_end,final_iter,crossover\n')
    statsfile.close()

def main():
    print("Starting games...")
    random.seed(3)

    init_logs()

    for PERCENT_EVIL in cfg.params_ranges['PERCENT_EVIL']:
        for PAYOFF in cfg.params_ranges['PERCENT_EVIL']:
            for WEALTH_GAP in cfg.params_ranges['WEALTH_GAP']:
                for SEC_INVESTMENT_CONVERSION_RATE in cfg.params_ranges['SEC_INVESTMENT_CONVERSION_RATE']:
                    for ATTACK_COST_CONVERSION_RATE in cfg.params_ranges['ATTACK_COST_CONVERSION_RATE']:
                        for CHANCE_OF_GETTING_CAUGHT in cfg.params_ranges['CHANCE_OF_GETTING_CAUGHT']:
                            for SEC_INVESTMENT in cfg.params_ranges['SEC_INVESTMENT']:
                                run_games(PERCENT_EVIL, PAYOFF, WEALTH_GAP, SEC_INVESTMENT_CONVERSION_RATE, ATTACK_COST_CONVERSION_RATE, CHANCE_OF_GETTING_CAUGHT, SEC_INVESTMENT)

    
if __name__== "__main__":
  main()
