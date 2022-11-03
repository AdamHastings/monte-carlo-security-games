from fileinput import filename
from Agent import *
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


'''
A Game object is a collection of const values plus the game results
'''
class Game:

    # TODO compute the init values...
    d_init = -1
    d_end = -1
    a_init = -1
    a_end = -1
    i_init = -1
    i_end = -1
    g_init = -1
    g_end = -1
    crossover = -1
    final_iter = -1

        
    def __init__(self, params, Attackers, Defenders, Insurer, Government):
        self.params = params
        self.Attackers = Attackers
        self.Defenders = Defenders
        self.Insurer = Insurer
        self.Government = Government

        self.d_init = self.sum_assets(Defenders)
        self.a_init = self.sum_assets(Attackers)
        self.i_init = Insurer.assets
        self.g_init = Government.assets


    def __str__(self):
        ret = ""
        ret += ",".join(str(self.params[k]) for k in sorted(self.params.keys())) + ","
        ret += str(self.d_init) + ","
        ret += str(self.d_end) + ","
        ret += str(self.a_init) + ","
        ret += str(self.a_end) + ","
        ret += str(self.i_init) + ","
        ret += str(self.i_end) + ","
        ret += str(self.crossover) + ","
        ret += str(self.final_iter) + "\n"
        return ret

        
    def fight(self, Defender, Attacker):

        effective_loot = Defender.assets * self.PAYOFF
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
            if (np.random.uniform(0,1) < self.CAUGHT):
                if (AttackerWins):
                    recoup_amount = effective_loot if Attacker.assets > effective_loot else Attacker.assets
                    Defender.recoup(recoup_amount) # Defender recoups amount that was lost
                    Attacker.lose(recoup_amount)

                # Remaining assets are seized by the government
                Government.gain(Attacker.assets)
                Attacker.lose(Attacker.assets)           


    def sum_assets(self, Players):
        total_assets = 0
        for x in Players:
            total_assets += x.assets

        return total_assets
                

    def prune(self):

        # TODO this is horribly inefficient. Redo
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

    def run_iterations(self):
        a_iters = []
        d_iters = []
        stats = []
        stability_reached = False
        stable_count = 0

        for iter_num in range(cfg.game_settings['SIM_ITERS']):

            random.shuffle(self.Defenders)

            for i in range(len(self.Attackers)):
                if (i > len(self.Defenders)):
                    print("more attackers than defenders!")
                    break
                self.fight(self.Defenders[i], self.Attackers[i])
            
            self.prune(self.Attackers, self.Defenders)

            a_sum = self.sum_assets(self.Attackers)
            d_sum = self.sum_assets(self.Defenders)
            
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

            if len(Defenders) == 0:
                final_iter = iter_num
                print("All Defenders dead after " + str(final_iter) + " iterations")
                break
            if len(Attackers) == 0:
                final_iter = iter_num
                print("All Attackers dead after " + str(final_iter) + " iterations")
                break
        
        if not stability_reached:
            stats.append((d_iters[0], d_sum , a_iters[0], a_sum, final_iter, crossover))
        
        return a_iters, d_iters, stats



def run_games(ATTACKERS, PAYOFF, INEQUALITY, EFFICIENCY, SUCCESS, CAUGHT, CLAIMS, PREMIUM, TAX, MANDATE):

    global cfg
    global gDefenders 
    global gAttackers
    global gInsurer
    global gGovernment

    params = {}
    params["ATTACKERS"] = ATTACKERS
    params["PAYOFF"] = PAYOFF
    params["INEQUALITY"] = INEQUALITY
    params["EFFICIENCY"] = EFFICIENCY
    params["SUCCESS"] = SUCCESS
    params["CAUGHT"] = CAUGHT
    params["CLAIMS"] = CLAIMS
    params["PREMIUM"] = PREMIUM
    params["TAX"] = TAX
    params["MANDATE"] = MANDATE


    for i in range(cfg.game_settings['NUM_GAMES']):
        # print("game " + str(i) + ":( " + str(ATTACKERS) + ", " + str(PAYOFF)+ ", " + str(INEQUALITY)+ ", " + str(EFFICIENCY)+ ", " + str(SUCCESS)+ ", " + str(CAUGHT) + ", " + str(MANDATE) + ")")
        
        # Create Agents here
        Defenders = deepcopy(gDefenders)
        Insurer = deepcopy(gInsurer)
        Government = deepcopy(gGovernment)
        Attackers = deepcopy(gAttackers[:int(cfg.game_settings["BLUE_PLAYERS"] * params["ATTACKERS"])])
        
        for a in Attackers:
            a.assets *= params['INEQUALITY']

        for d in Defenders:
            investment = d.assets * params["MANDATE"]
            d.lose(investment)
            
            insurance_investment = investment * params["PREMIUM"]
            d.buy_insurance(insurance_investment)
            Insurer.collect_premium(insurance_investment)

            sec_investment = investment - insurance_investment
            d.costToAttack += (sec_investment * params["EFFICIENCY"])

        # Game object to hold const game parameters
        g = Game(params, Attackers, Defenders, Insurer, Government)
        
        # g.run_iterations()
        
        log = open(cfg.LOGFILE, 'a')  # write mode
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

    log = open(cfg.LOGFILE, 'w')  # write mode
    header = ""
    for k in sorted(cfg.params_ranges.keys()):
        header += k[:-6] + "," # trim off the "_range" of the cfg param names
    header += "d_init,d_end,a_init,a_end,final_iter,crossover\n"
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

    inputs = list(itertools.product(cfg.params_ranges["ATTACKERS_range"],
                                        cfg.params_ranges["PAYOFF_range"],
                                        cfg.params_ranges["INEQUALITY_range"],
                                        cfg.params_ranges["EFFICIENCY_range"],
                                        cfg.params_ranges["SUCCESS_range"],
                                        cfg.params_ranges["CAUGHT_range"],
                                        cfg.params_ranges["CLAIMS_range"],
                                        cfg.params_ranges["PREMIUM_range"],
                                        cfg.params_ranges["TAX_range"],
                                        cfg.params_ranges["MANDATE_range"]
                                    ))

    print("Starting games...")
    with Pool(initializer=init_worker, initargs=(cfg,)) as p:
        p.starmap(run_games, inputs)

    print("\nFinished!\n")
    
if __name__== "__main__":
  main()
