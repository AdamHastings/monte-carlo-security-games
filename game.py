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

Defenders = []
Attackers = []
blue_dist=None
red_dist=None
d_iters = []
a_iters = []
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
        skills = np.random.normal(0,1)
        # assets = cfg.blue['ASSETS']
        randwealth = [random.randint(0,9999), random.randint(10000,99999), random.randint(100000,999999), random.randint(1000000,9999999)]
        assets = choice(randwealth, 1, p=[0.55, 0.33, 0.11, 0.01])[0]
    elif agent=="red":
        # skills = random.choice(red_dist)
        randwealth = [random.randint(0,9999), random.randint(10000,99999), random.randint(100000,999999), random.randint(1000000,9999999)]        
        skills = np.random.normal(0,1)
        # assets = cfg.red['ASSETS']
        assets = choice(randwealth, 1, p=[0.55, 0.33, 0.11, 0.01])[0] * cfg.params['WEALTH_GAP']


    else:
        sys.exit(0)

    return assets, skills

def init_game():

    Defenders.clear()
    Attackers.clear()
    d_iters.clear()
    a_iters.clear()

    #assets_dist = []
    
    for _ in range(BLUETEAM_SIZE):
        assets, skills = get_agent_params("blue")
        #assets_dist.append(assets)
        x = Defender(assets, skills)
        Defenders.append(x)

    #print("plotting hist")
    #plt.hist(assets_dist, bins=[0, 10000, 100000, 1000000, 10000000])
    #plt.title("Histogram")
    #plt.xscale("log")
    #plt.show()
    #plt.clf()


    #assets_dist = []
    for _ in range(REDTEAM_SIZE):
        assets, skills = get_agent_params("red")
        #assets_dist.append(assets)
        x = Attacker(assets, skills)
        Attackers.append(x)

    #print("plotting hist")
    #plt.hist(assets_dist, bins=[0, 10000 * cfg.game['WEALTH_GAP'], 100000 * cfg.game['WEALTH_GAP'], 1000000 * cfg.game['WEALTH_GAP'], 10000000 * cfg.game['WEALTH_GAP']], color="red")
    #plt.title("Histogram")
    #plt.xscale("log")
    #plt.show()
    #plt.clf()




def compute_utility():
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

def fight(Defender, Attacker):

    # possible_earnings = Defender.assets * cfg.game['PAYOFF']
    # cost_of_attack = 

    if ((Defender.skill) < (Attacker.skill)):
        effective_loot = Defender.assets * cfg.params['PAYOFF']
        Defender.lose(effective_loot)
        Attacker.win(effective_loot, cfg.params['COST_TO_ATTACK'])
    else:
        Attacker.lose(cfg.params['COST_TO_ATTACK'])

def prune():
    global Defenders
    global Attackers

    Temp = []
    for i in range(len(Defenders)):
        #if d.get_assets() <= 0:
        #    Defenders.remove(Defender)
        if Defenders[i].get_assets() > 0:
            Temp.append(Defenders[i])

    Defenders = Temp

    Temp = []
    for i in range(len(Attackers)):
        if Attackers[i].get_assets() > cfg.params['COST_TO_ATTACK']:
            Temp.append(Attackers[i])

    Attackers = Temp

def run_iterations():
    for iter_num in range(cfg.game_settings['SIM_ITERS']):
        global Defenders

        # print(Defenders)
        random.shuffle(Defenders)

        for i in range(len(Attackers)):
            if (i > len(Defenders)):
                print("more attackers than defenders!")
                break
            fight(Defenders[i], Attackers[i])
        
        prune()

        for i in range(len(Defenders)):
            Defenders[i].assets += cfg.blue['EARNINGS']
        
        compute_utility()
        if len(Defenders) is 0:
            print("All Defenders dead")
            return iter_num
        if len(Attackers) is 0:
            print("All Attackers dead")
            return iter_num

def plot_results(i):
    plt.plot(d_iters, label="defenders, game " + str(i), linewidth=2, linestyle=linestyles[i], color="b")
    plt.plot(a_iters, label="attackers, game " + str(i), linewidth=2, linestyle=linestyles[i], color="r")
    # plt.xlim([0,5000])



def get_dist(team):
    if team=="blue":
        teamparams = cfg.blue
    elif team=="red":
        teamparams = cfg.red
    else:
        sys.exit(0)

    if teamparams['dist'] == "normal":
        mu = teamparams['mu']
        sigma = teamparams['sigma']
        scale = teamparams['scale']  
        dist = np.random.normal(mu,sigma,1000000)
    elif teamparams['dist'] == "lognormal":
        mu = teamparams['mu']
        sigma = teamparams['sigma']
        scale = teamparams['scale']  
        dist = np.random.lognormal(mu,sigma,1000000)
    elif teamparams['dist'] == "skew":
        a = teamparams['a']
        dist = skewnorm.rvs(a, size=100000)
        scale = teamparams['scale']  
    else:
        sys.exit(0)
    dist = dist / np.average(dist)
    dist = (dist - min(dist))/(max(dist) - min(dist))
    dist = dist * scale

    return dist


def run_games():
    # plt.subplot(3,1,1)
    global blue_dist
    blue_dist = get_dist("blue")
    # plt.xlim([0,10000])
    # plt.title("Blue Team")
    # plt.xlabel("assets")
    # plt.hist(blue_dist, bins=500, color="b")
    # plt.subplot(3,1,2)
    
    global red_dist
    red_dist = get_dist("red")
    # plt.xlim([0,10000])
    # plt.title("Red Team")
    # plt.xlabel("assets")
    # plt.hist(red_dist, bins=500, color="r")
    # plt.subplot(3,1,3)

    bavg = np.empty((cfg.game_settings['NUM_GAMES'], cfg.game_settings['SIM_ITERS']))
    ravg = np.empty((cfg.game_settings['NUM_GAMES'], cfg.game_settings['SIM_ITERS']))


    for i in range(cfg.game_settings['NUM_GAMES']):
        init_game()
        final = run_iterations()
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

        bavg[i] = d_iters
        ravg[i] = a_iters

    bavg = np.average(bavg, axis=0)
    ravg = np.average(ravg, axis=0)
    print(bavg)
    plt.plot(bavg, label="Blue Team Average", linewidth=2, linestyle="-", color="b")
    plt.plot(ravg, label="Red Team Average", linewidth=2, linestyle="-", color="r")


    plt.xlabel("iterations")
    plt.ylabel("total assets")
    # plt.ylim(0, 200000)
    plt.legend()
    plt.show()


def main():
    # init_env()
    print("Starting games...")
    run_games()

    
  
if __name__== "__main__":
  main()
