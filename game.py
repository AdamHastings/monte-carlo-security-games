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
    def __init__(self, Defenders, Attackers, ATTACKERS, PAYOFF, INEQUALITY, EFFICIENCY, SUCCESS, CAUGHT, CLAIMS, PREMIUM, TAX, MANDATE):
        self.ATTACKERS = ATTACKERS
        self.PAYOFF = PAYOFF
        self.INEQUALITY = INEQUALITY
        self.EFFICIENCY = EFFICIENCY
        self.SUCCESS = SUCCESS
        self.CAUGHT = CAUGHT
        self.CLAIMS = CLAIMS
        self.PREMIUM = PREMIUM
        self.TAX = TAX
        self.MANDATE = MANDATE
        self.d_init = -1
        self.d_end = -1
        self.a_init = -1
        self.a_end = -1
        self.i_init = -1
        self.i_end = -1
        self.crossover = -1
        self.final_iter = -1

    def __str__(self):
        ret = ""
        ret += self.ATTACKERS + ","
        ret += self.PAYOFF + ","
        ret += self.INEQUALITY  + ","
        ret += self.EFFICIENCY  + ","
        ret += self.SUCCESS  + ","
        ret += self.CAUGHT  + ","
        ret += self.CLAIMS  + ","
        ret += self.PREMIUM  + ","
        ret += self.TAX  + ","
        ret += self.MANDATE  + ","
        ret += self.d_init + ","
        ret += self.d_end + ","
        ret += self.a_init + ","
        ret += self.a_end + ","
        ret += self.i_init + ","
        ret += self.i_end + ","
        ret += self.crossover + ","
        ret += self.final_iter + ","
        return ret

    def fight(self, Defender, Attacker, g):

        effective_loot = Defender.assets * g.PAYOFF
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
            if (np.random.uniform(0,1) < g.CAUGHT):
                if (AttackerWins):
                    recoup_amount = effective_loot if Attacker.assets > effective_loot else Attacker.assets
                    Defender.recoup(recoup_amount) # Defender recoups amount that was lost
                    Attacker.lose(recoup_amount)

                Attacker.lose(Attacker.assets)           # Remaining assets are seized by the government
                # TODO add Gov player
                # Government.gain(Attacker.assets)
                

    def prune(self, Attackers, Defenders):

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

    def run_iterations(self, Attackers, Defenders, g):
        crossover = -1
        final_iter = -1

        a_iters = []
        d_iters = []
        stats = []
        stability_reached = False
        stable_count = 0

        for iter_num in range(cfg.game_settings['SIM_ITERS']):

            random.shuffle(Defenders)

            for i in range(len(Attackers)):
                if (i > len(Defenders)):
                    print("more attackers than defenders!")
                    break
                self.fight(Defenders[i], Attackers[i], g.PAYOFF, g.CAUGHT)
            
            self.prune(Attackers, Defenders)

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

    def init_game(self, g):

        Defenders = []    
        for _ in range(int(cfg.BLUE_PLAYERS)):
            Defenders.append(create_blue_agent(g.SUCCESS))
        
        Attackers = []
        for _ in range(int(cfg.BLUE_PLAYERS * g.ATTACKERS)):
            Attackers.append(create_red_agent(g.INEQUALITY))

        Insurer = create_insurer(Defenders, g.PREMIUM)
        Government = create_government()

        ### TODO Split below this line into a game-specific init..?
        for d in Defenders:
            investment = d.assets * g.MANDATE
            d.assets -= investment
            d.costToAttack += (d.assets * g.MANDATE * g.EFFICIENCY)

        return Attackers, Defenders, Insurer, Government


    def sum_assets(self, Players):
        total_assets = 0
        for x in Players:
            total_assets += x.assets

        return total_assets


# def compute_utility(Attackers, Defenders, a_iters, d_iters):
#     defenders_assets = 0
#     for x in Defenders:
#         defenders_assets += x.assets
#     d_iters.append(defenders_assets)

#     attackers_assets = 0
#     for x in Attackers:
#         attackers_assets += x.assets
#     a_iters.append(attackers_assets)

#     return defenders_assets, attackers_assets
    



def run_games(ATTACKERS, PAYOFF, INEQUALITY, EFFICIENCY, SUCCESS, CAUGHT, CLAIMS, PREMIUM, TAX, MANDATE):

    # Game object to hold const game parameters
    g = Game(ATTACKERS=ATTACKERS, \
                PAYOFF=PAYOFF, \
                INEQUALITY=INEQUALITY, \
                EFFICIENCY=EFFICIENCY, \
                SUCCESS=SUCCESS, \
                CAUGHT=CAUGHT, \
                CLAIMS=CLAIMS, \
                PREMIUM=PREMIUM, \
                TAX=TAX, \
                MANDATE=MANDATE
            )

    for i in range(cfg.game_settings['NUM_GAMES']):
        # print("game " + str(i) + ":( " + str(ATTACKERS) + ", " + str(PAYOFF)+ ", " + str(INEQUALITY)+ ", " + str(EFFICIENCY)+ ", " + str(SUCCESS)+ ", " + str(CAUGHT) + ", " + str(MANDATE) + ")")
        
        # TODO maybe optimize by creating template Defender/Attacker 
        red, blue = init_game(g)

        # TODO Why are we doing this?
        Attackers = deepcopy(red)
        Defenders = deepcopy(blue)
        
        run_iterations(Attackers, Defenders, g)
        
        statsfile = open(cfg.FILENAME, 'a')  # write mode
        statsfile.write(g)
        statsfile.close()



def init_logs(cfg):
    if os.path.exists(cfg.FILENAME):
        response = input("\nThis file already exists: " + cfg.FILENAME + "\nDo you want to replace it? Y/n\n >> ")
        if response.lower() != "y":
            print("\nOK, this program will not overwrite " + cfg.FILENAME + ".\nThis program will now exit.\n")
            sys.exit(0)
        else:
            print("\n")

    statsfile = open(cfg.FILENAME, 'w')  # write mode
    header = ",".join(cfg.params.keys()) + ","
    header += "d_init,d_end,a_init,a_end,final_iter,crossover\n"
    statsfile.write(header)
    statsfile.close()

'''Initialize worker processes'''
def init_worker(cfg_in):
    global cfg
    cfg = cfg_in

    global gDefenders 
    global gAttackers
    global gInsurer
    global gGovernment

def main():
    random.seed(3)

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

    ATTACKERS_range = cfg.params["ATTACKERS"]
    PAYOFF_range = cfg.params["PAYOFF"]
    INEQUALITY_range = cfg.params["INEQUALITY"]
    EFFICIENCY_range = cfg.params["EFFICIENCY"]
    SUCCESS_range = cfg.params["SUCCESS"]
    CAUGHT_range = cfg.params["CAUGHT"]
    CLAIMS_range = cfg.params["CLAIMS"]
    PREMIUM_range = cfg.params["PREMIUM"]
    TAX_range = cfg.params["TAX"]
    MANDATE_range = cfg.params["MANDATE"]

    inputs = list(itertools.product(ATTACKERS_range,\
                                        PAYOFF_range,\
                                        INEQUALITY_range,\
                                        EFFICIENCY_range,
                                        SUCCESS_range,\
                                        CAUGHT_range,\
                                        CLAIMS_range,\
                                        PREMIUM_range,\
                                        TAX_range,\
                                        MANDATE_range\
                                    ))

    print("Starting games...")
    with Pool(initializer=init_worker, initargs=(cfg,)) as p:
        p.starmap(run_games, inputs)

    print("\nFinished!\n")
    
if __name__== "__main__":
  main()
