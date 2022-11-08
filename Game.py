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
        
    def __init__(self, game_settings, params, Attackers, Defenders, Insurer, Government):
        self.params = params
        self.game_settings = game_settings
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

        self.last_delta_defenders_changes = [self.game_settings["EPSILON_DOLLARS"]] * self.game_settings["DELTA_ITERS"]
        self.last_delta_attackers_changes = [self.game_settings["EPSILON_DOLLARS"]] * self.game_settings["DELTA_ITERS"]

        self.outside_epsilon_count_attackers = self.game_settings["DELTA_ITERS"]
        self.outside_epsilon_count_defenders = self.game_settings["DELTA_ITERS"]

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

        last_delta_defenders_pop = self.last_delta_defenders_changes[self.iter_num % self.game_settings["DELTA_ITERS"]]
        last_delta_attackers_pop = self.last_delta_attackers_changes[self.iter_num % self.game_settings["DELTA_ITERS"]]
        self.last_delta_defenders_changes[self.iter_num % self.game_settings["DELTA_ITERS"]] = self.defender_iter_sum
        self.last_delta_attackers_changes[self.iter_num % self.game_settings["DELTA_ITERS"]] = self.attacker_iter_sum
     
        if self.defender_iter_sum >= self.game_settings["EPSILON_DOLLARS"]:
            if last_delta_defenders_pop < self.game_settings["EPSILON_DOLLARS"]:
                self.outside_epsilon_count_defenders += 1
        else:
            if last_delta_defenders_pop >= self.game_settings["EPSILON_DOLLARS"]:
                self.outside_epsilon_count_defenders -= 1

        if self.attacker_iter_sum >= self.game_settings["EPSILON_DOLLARS"]:
            if last_delta_attackers_pop < self.game_settings["EPSILON_DOLLARS"]:
                self.outside_epsilon_count_attackers += 1
        else:
            if last_delta_attackers_pop >= self.game_settings["EPSILON_DOLLARS"]:
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
        # add 1 here and there to account for floating point imprecsion
        assert self.d_end + 1 >= 0, f'{self.params}'
        assert self.a_end + 1 >= 0, f'{self.params}'
        assert self.current_defender_sum_assets + 1 >= 0, f'{self.params}, {self.current_defender_sum_assets}'
        assert self.current_attacker_sum_assets + 1 >= 0, f'{self.params}'
        assert abs(self.d_end - self.current_defender_sum_assets) < 1, f'{self.params}'
        assert abs(self.a_end - self.current_attacker_sum_assets) < 1, f'{self.params}'
        assert self.d_init >= self.d_end, f'{self.params}'
        assert self.paid_claims <= (self.i_init + 1) and self.paid_claims >=0, f'{self.params}'


        self.i_end = self.Insurer.assets
        self.g_end = self.Government.assets


        self.final_iter = self.iter_num
        if len(self.Defenders) > 0 and len(self.Attackers) > 0:
            assert self.final_iter >= self.game_settings['DELTA_ITERS'], f'{self.params}'

        if self.crossover >= 0:
            assert self.d_end < self.a_end, f'{self.params}'

        self.reason = reason
  
    def run_iterations(self):

        for self.iter_num in range(1, self.game_settings['SIM_ITERS']+1):

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
