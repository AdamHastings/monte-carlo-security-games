from fileinput import filename
from datetime import datetime
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
        
    def __init__(self, params, Attackers, Defenders, Insurer, Government):
        self.params = params
        self.Defenders = Defenders
        self.Attackers = Attackers
        self.Insurer = Insurer
        self.Government = Government

        self.d_init = sum(d.assets for d in self.Defenders)
        self.a_init = sum(a.assets for a in self.Attackers)
        self.i_init = Insurer.assets
        self.g_init = Government.assets

        self.crossover = -1
        self.insurer_time_of_death = -1
        self.paid_claims = 0

        self.current_defender_sum_assets = self.d_init
        self.current_attacker_sum_assets = self.a_init

        self.defender_iter_sum = 0
        self.attacker_iter_sum = 0

        self.last_delta_defenders_changes = [cfg.game_settings["EPSILON_DOLLARS"]] * cfg.game_settings["DELTA_ITERS"]
        self.last_delta_attackers_changes = [cfg.game_settings["EPSILON_DOLLARS"]] * cfg.game_settings["DELTA_ITERS"]

        self.outside_epsilon_count_attackers = cfg.game_settings["DELTA_ITERS"]
        self.outside_epsilon_count_defenders = cfg.game_settings["DELTA_ITERS"]

    def __str__(self):
        ret = ""
        ret += ",".join(str(round(self.params[k], 2)).lstrip('0') for k in sorted(self.params.keys())) + ","
        ret += str(round(self.d_init)) + ","
        ret += str(round(self.d_end)) + ","
        ret += str(round(self.a_init)) + ","
        ret += str(round(self.a_end)) + ","
        ret += str(round(self.i_init)) + ","
        ret += str(round(self.i_end)) + ","
        ret += str(self.crossover) + ","
        ret += str(self.insurer_time_of_death) + ","
        ret += str(round(self.paid_claims)) + ","
        ret += str(self.final_iter) + ","
        ret += str(self.reason) + "\n"
        return ret

    def defender_lose(self, d, loss):
        d.lose(loss)
        self.defender_iter_sum -= loss

    def defender_gain(self, d, gain):
        d.gain(gain)
        self.defender_iter_sum += gain

    def attacker_lose(self, a, loss):
        a.lose(loss)
        self.attacker_iter_sum -= loss

    def attacker_gain(self, a, gain):
        a.gain(gain)
        self.attacker_iter_sum += gain

    def insurer_lose(self, i, loss):
        i.lose(loss)
        self.paid_claims += loss

    def government_gain(self, g, gain):
        g.gain(gain)
        
    def fight(self, a, d):

        effective_loot = d.assets * self.params["PAYOFF"]
        cost_of_attack = d.costToAttack
        expected_earnings = effective_loot * d.ProbOfAttackSuccess

        if (expected_earnings > cost_of_attack) and (cost_of_attack < a.assets):
            # Attacker decides that it's worth it to attack

            # Pay the cost of attacking
            self.attacker_lose(a, cost_of_attack)
     
            # The attacker might get caught
            if (np.random.uniform(0,1) < self.params["CAUGHT"]):
                # if (AttackerWins):
                #     recoup_amount = effective_loot if a.assets > effective_loot else a.assets
                #     self.defender_gain(d, recoup_amount)
                #     self.attacker_lose(a, recoup_amount)
    
                # Remaining assets are seized by the government
                self.government_gain(self.Government, a.assets)
                self.attacker_lose(a, a.assets)
                # TODO distribute confiscated earnings to victims

            else:
                AttackerWins = (np.random.uniform(0,1) < d.ProbOfAttackSuccess)
                if (AttackerWins):
                    self.defender_lose(d, effective_loot)
                    self.attacker_gain(a, effective_loot)
                    
                    # Note: we do not re-scale a defender's costToAttack to be proportionate to the new level of assets
                    # This is because we assume previous security investments are still valid!
                    
                    # Recoup losses from Insurer (if Insurer is not dead yet)
                    if self.Insurer.assets > 0:
                        # The defender gets to recoup losses from Insurer
                        claims_amount = effective_loot * self.params["CLAIMS"]
                        if (claims_amount > self.Insurer.assets):
                            # Insurer goes bust
                            claims_amount = self.Insurer.assets
                            self.insurer_time_of_death = self.iter_num
                                    
                        self.defender_gain(d, claims_amount) 
                        self.insurer_lose(self.Insurer, claims_amount)

    def is_equilibrium_reached(self):

        last_delta_defenders_pop = self.last_delta_defenders_changes[self.iter_num % cfg.game_settings["DELTA_ITERS"]]
        last_delta_attackers_pop = self.last_delta_attackers_changes[self.iter_num % cfg.game_settings["DELTA_ITERS"]]
        self.last_delta_defenders_changes[self.iter_num % cfg.game_settings["DELTA_ITERS"]] = self.defender_iter_sum
        self.last_delta_attackers_changes[self.iter_num % cfg.game_settings["DELTA_ITERS"]] = self.attacker_iter_sum
     
        if self.defender_iter_sum >= cfg.game_settings["EPSILON_DOLLARS"]:
            if last_delta_defenders_pop < cfg.game_settings["EPSILON_DOLLARS"]:
                self.outside_epsilon_count_defenders += 1
        else:
            if last_delta_defenders_pop >= cfg.game_settings["EPSILON_DOLLARS"]:
                self.outside_epsilon_count_defenders -= 1

        if self.attacker_iter_sum >= cfg.game_settings["EPSILON_DOLLARS"]:
            if last_delta_attackers_pop < cfg.game_settings["EPSILON_DOLLARS"]:
                self.outside_epsilon_count_attackers += 1
        else:
            if last_delta_attackers_pop >= cfg.game_settings["EPSILON_DOLLARS"]:
                self.outside_epsilon_count_attackers -= 1

        assert self.outside_epsilon_count_attackers >= 0, f'{self.params}'
        assert self.outside_epsilon_count_defenders >= 0, f'{self.params}'

        return self.outside_epsilon_count_attackers == 0 and self.outside_epsilon_count_defenders == 0

    '''
    reason: A code as to why this particular game has ended
    possible values:
        A: Attackers all dead
        D: Defenders all dead
        E: Equilibrium reached
        N: Non-convergence (none of the above)
    '''
    def conclude_game(self, reason):
        self.d_end = sum(d.assets for d in self.Defenders)  
        self.a_end = sum(a.assets for a in self.Attackers)

        # Make sure the game has ended in a sane state
        assert self.d_end >= 0, f'{self.params}'
        assert self.a_end >= 0, f'{self.params}'
        assert self.current_defender_sum_assets >= 0, f'{self.params}'
        assert self.current_attacker_sum_assets >= 0, f'{self.params}'
        assert abs(self.d_end - self.current_defender_sum_assets) < 1, f'{self.params}'
        assert abs(self.a_end - self.current_attacker_sum_assets) < 1, f'{self.params}'
        assert self.d_init >= self.d_end, f'{self.params}'

        self.i_end = self.Insurer.assets
        self.g_end = self.Government.assets

        assert self.paid_claims <= self.i_init and self.paid_claims >=0, f'{self.params},  {self.paid_claims}, {self.i_init}'

        self.final_iter = self.iter_num
        if len(self.Defenders) > 0 and len(self.Attackers) > 0:
            assert self.final_iter >= cfg.game_settings['DELTA_ITERS'], f'{self.params}'

        if self.crossover >= 0:
            assert self.d_end < self.a_end, f'{self.params}'

        self.reason = reason
  
    def run_iterations(self):

        for self.iter_num in range(1, cfg.game_settings['SIM_ITERS']+1):

            # Make the pairings between Attackers and Defenders random
            random.shuffle(self.Attackers)

            self.defender_iter_sum = 0
            self.attacker_iter_sum = 0

            for a,d in zip(self.Attackers, self.Defenders):
                self.fight(a=a, d=d)

            self.current_defender_sum_assets += self.defender_iter_sum
            self.current_attacker_sum_assets += self.attacker_iter_sum

            # Check if Attackers now own more than Defenders
            if (self.current_attacker_sum_assets > self.current_defender_sum_assets):
                self.crossover = self.iter_num

            # Remove the dead players from the game
            self.Defenders = [d for d in self.Defenders if d.assets > 0]
            self.Attackers = [a for a in self.Attackers if a.assets > 0]                
            
            # Check if the game needs to be ended

            # Condition #1: Either the Defenders or the Attackers completely die off
            if len(self.Defenders) == 0:
                self.conclude_game('A')
                return
            
            if len(self.Attackers) == 0:
                self.conclude_game('D')
                return

            # Condition #2: The game has converged and hasn't changed by epsilon for delta iterations
            if self.is_equilibrium_reached():
                # Game has reach an equilibrium
                self.conclude_game('E')
                return
        
        # Condition #3: The game did not converge after SIM_ITERS iterations 
        self.conclude_game('N')
        return

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
        # print(params)
        
        # Create Agents here
        Defenders = deepcopy(gDefenders)
        Insurer = deepcopy(gInsurer)
        Government = deepcopy(gGovernment)
        Attackers = deepcopy(gAttackers[:int(cfg.game_settings["BLUE_PLAYERS"] * params["ATTACKERS"])])
        
        for a in Attackers:
            a.assets *= params['INEQUALITY']
            assert a.assets > 0, f'{params}'

        for d in Defenders:
            investment = d.assets * params["MANDATE"]
            
            insurance_investment = investment * params["PREMIUM"]
            d.lose(insurance_investment)
            Insurer.gain(insurance_investment)

            sec_investment = investment - insurance_investment
            d.lose(sec_investment)
            d.costToAttack = d.assets * params["SUCCESS"]
            d.costToAttack += (sec_investment * params["EFFICIENCY"])

            assert d.assets >=0, f'{params}'

        # Create a Game object to hold game parameters
        g = Game(params=params, Attackers=Attackers, Defenders=Defenders, Insurer=Insurer, Government=Government)
        
        g.run_iterations()
        
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
    print("Writing to", cfg.LOGFILE)

    log = open(cfg.LOGFILE, 'w')  # write mode
    header = ""
    for k in sorted(cfg.params_ranges.keys()):
        header += k[:-6] + "," # trim off the "_range" of the cfg param names
    header += "d_init,d_end,a_init,a_end,i_init,i_end,crossover,insurer_tod,paid_claims,final_iter,reason\n"
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

    now = datetime.now()
    current_time = now.strftime("%H:%M:%S")

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

    print("Starting", len(inputs), "games at", current_time)
    with Pool(initializer=init_worker, initargs=(cfg,)) as p:
        p.starmap(run_games, inputs)

    now = datetime.now()
    current_time = now.strftime("%H:%M:%S")
    print("\nFinished! at", current_time)
    
if __name__== "__main__":
  main()
