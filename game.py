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


def get_agent_params(agent):
    if agent=="blue":
        assets = random.choice(blue_dist)
    elif agent=="red":
        assets = random.choice(red_dist)
    else:
        sys.exit(0)

    skills = np.random.normal()
    return assets, skills

def init_game():

    Defenders.clear()
    Attackers.clear()
    d_iters.clear()
    a_iters.clear()

    for _ in range(cfg.game['BLUETEAM_SIZE']):
        assets, skills = get_agent_params("blue")
        x = Defender(assets, skills)
        Defenders.append(x)

    for _ in range(cfg.game['REDTEAM_SIZE']):
        assets, skills = get_agent_params("red")
        x = Attacker(assets, skills)
        Attackers.append(x)


def compute_utility():
    defenders_assets = 0
    for x in Defenders:
        defenders_assets += x.assets
    d_iters.append(defenders_assets)

    attackers_assets = 0
    for x in Attackers:
        attackers_assets += x.assets
    a_iters.append(attackers_assets)

def fight(Defender, Attacker):
    if ((Defender.skill) < (Attacker.skill)):
        real_loot = Defender.lose(cfg.game['LOOT'])
        Attacker.win(real_loot, cfg.game['COST_TO_ATTACK'])
    else:
        Attacker.lose(cfg.game['COST_TO_ATTACK'])
    
    if Defender.get_assets() < cfg.game['LOOT']:
        Defenders.remove(Defender)

    if Attacker.get_assets() < cfg.game['COST_TO_ATTACK']:
        Attackers.remove(Attacker)

def run_iterations():
    for iter_num in range(cfg.game['SIM_ITERS']):

        random.shuffle(Defenders)

        for i in range(len(Attackers)):
            fight(Defenders[i], Attackers[i])
        
        Defenders += cfg.blue['E']
        
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
        dist = np.random.normal(mu,sigma,100000)
    elif teamparams['dist'] == "lognormal":
        mu = teamparams['mu']
        sigma = teamparams['sigma']
        scale = teamparams['scale']  
        dist = np.random.lognormal(mu,sigma,100000)
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
    plt.subplot(3,1,1)
    global blue_dist
    blue_dist = get_dist("blue")
    plt.xlim([0,10000])
    plt.title("Blue Team")
    plt.xlabel("assets")
    plt.hist(blue_dist, bins=500, color="b")
    plt.subplot(3,1,2)
    
    global red_dist
    red_dist = get_dist("red")
    plt.xlim([0,10000])
    plt.title("Red Team")
    plt.xlabel("assets")
    plt.hist(red_dist, bins=500, color="r")
    plt.subplot(3,1,3)

    for i in range(cfg.game['NUM_GAMES']):
        init_game()
        final = run_iterations()
        print("Finished after " + str(final) + " iterations")
        plot_results(i)

    plt.xlabel("iterations")
    plt.ylabel("total assets")
    plt.legend()
    plt.show()


def main():
    # init_env()
    print("Starting games...")
    run_games()
    
  
if __name__== "__main__":
  main()