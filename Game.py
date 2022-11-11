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

        # print(f'g_init={self.g_init}')

        # Some interesting stats to keep track of
        self.crossovers = []
        self.insurer_time_of_death = -1
        self.paid_claims = 0
        self.attacks = 0
        self.outcome = "X"
        self.caught = 0
        self.attacker_expenditures = 0
        self.amount_stolen = 0

        # Keep track of the cumulative worth of Attackers and Defenders assets (e.g. memoize)
        self.current_defender_sum_assets = self.d_init
        self.current_attacker_sum_assets = self.a_init

        # Counter for each iteration of each game
        self.defender_iter_sum = 0
        self.attacker_iter_sum = 0

        # Keep track of the last DELTA_ITERS outcomes to check if the game is at an equilibrium or not
        self.last_delta_defenders_changes = [self.game_settings["EPSILON_DOLLARS"]] * self.game_settings["DELTA_ITERS"]
        self.last_delta_attackers_changes = [self.game_settings["EPSILON_DOLLARS"]] * self.game_settings["DELTA_ITERS"]
        
        # How many of the last Agent changes were outside of the EPSILON_DOLLARS threshold (?)
        self.outside_epsilon_count_attackers = self.game_settings["DELTA_ITERS"]
        self.outside_epsilon_count_defenders = self.game_settings["DELTA_ITERS"]

    def __str__(self):
        ret = ""
        ret += ",".join(str(round(self.params[k], 2)).lstrip('0') for k in sorted(self.params.keys())) + ","
        ret += str(round(self.d_init)) + ","
        ret += str(round(self.current_defender_sum_assets))  + ","
        ret += str(round(self.a_init)) + ","
        ret += str(round(self.current_attacker_sum_assets))  + ","
        ret += str(round(self.i_init)) + ","
        ret += str(round(self.Insurer.assets))  + ","
        ret += str(round(self.g_init)) + ","
        ret += str(round(self.Government.assets))  + ","
        ret += str(self.attacks) + ","
        ret += str(self.amount_stolen) + ","
        ret += str(self.attacker_expenditures) + ","
        ret += "\"" + str(self.crossovers) + "\","
        ret += str(self.insurer_time_of_death) + "," # TODO make this insurer_times_of_death (note the "s")
        ret += str(round(self.paid_claims)) + ","
        ret += str(self.iter_num) + ","
        ret += str(self.outcome) + "\n"
        return ret

    '''
    outcome: A code as to why this particular game has ended
    possible values:
        A: Attackers all dead
        D: Defenders all dead
        E: Equilibrium reached
        N: Non-convergence (none of the above)
    '''
    def verify_state(self):

        # Make sure the game has ended in a sane state
        # add 1 here and there to account for floating point imprecsion
        assert self.current_defender_sum_assets + 1 >= 0, f'{self.params}'
        assert self.current_attacker_sum_assets + 1 >= 0, f'{self.params}'
        assert self.Insurer.assets + 1 >= 0, f'{self.params}'
        assert self.Government.assets + 1 >= 0, f'{self.params}'
        assert abs(self.current_defender_sum_assets - sum(d.assets for d in self.Defenders)) + 1 >= 0, f'{self.params}'
        assert abs(self.current_attacker_sum_assets - sum(a.assets for a in self.Attackers)) + 1 >= 0, f'{self.params}'
        assert self.i_init + 1 > self.Insurer.assets, f'{self.params}'
        assert self.d_init + 1 > self.current_defender_sum_assets, f'{self.params}'
        assert self.paid_claims <= (self.i_init + 1) and self.paid_claims + 1 >= 0, f'{self.params}, {self.i_init}, {self.paid_claims}'
        assert self.amount_stolen >= self.paid_claims, f'{self.params},'

        if self.outcome == 'E':
            assert len(self.alive_attackers) > 0, f'{self.params}'
            assert len(self.alive_defenders) > 0, f'{self.params}'
            assert self.iter_num >= self.game_settings['DELTA_ITERS'], f'{self.params}'
        elif self.outcome == 'D':
            assert len(self.alive_defenders) == 0, f'{self.params}'
        elif self.outcome == 'A':
            assert len(self.alive_attackers) == 0, f'{self.params}'

        assert ((self.d_init + self.a_init + self.g_init + self.i_init) - (self.current_defender_sum_assets + self.current_attacker_sum_assets + self.Insurer.assets + self.Government.assets + self.attacker_expenditures)) + 1 >= 0, f'\n\nMaster checksum failed!\n\n{str(self)}, {self.params},'

    def conclude_game(self, outcome):
        self.outcome = outcome
        self.verify_state()


    def a_steals_from_d(self, a, d, loot):
        self.defender_lose(d, loot)
        self.attacker_gain(a, loot)
        self.amount_stolen += loot

        if d.id in a.victims:
            a.victims[d.id] += loot
        else:
            a.victims[d.id] = loot

    def defender_gain(self, d, gain):
        d.gain(gain)
        self.defender_iter_sum += gain

    def defender_lose(self, d, loss):
        d.lose(loss)
        self.defender_iter_sum -= loss
        # print(f'** defender_iter_sum: {self.defender_iter_sum}')
        assert d.assets + 1 >= 0, f'{self.params},'

    def attacker_lose(self, a, loss):
        a.lose(loss)
        self.attacker_iter_sum -= loss
        assert a.assets + 1 >= 0, f'{self.params},'

    def attacker_gain(self, a, gain):
        a.gain(gain)
        self.attacker_iter_sum += gain

    def defender_recoup(self, a, d, recoup):
        # Check if the defender has been paid claims for losses to a
        # print(f'    d.claims_received: {d.claims_received}')
        # print(f'    has d has been attacked by Attackers[{self.a_i}]?')
        # print("      recouping ", recoup)
        if a.id in d.claims_received:
            # print(f'      yes')
            claims_received_from_a = d.claims_received[a.id]
            # Pay back received claims to insurer first
            # TODO reduce d.claims_received by the appropriate amount
            if recoup >= claims_received_from_a:
                # print("      defender gets to recoup some as well")
                # Defender gets to keep some
                # TODO Make sure you revive Insurer...maybe may TOD a list?
                self.defender_gain(d, gain=recoup - claims_received_from_a)
                # Pay back the Insurer as much as d is able
                # TODO don't let a pay Insurer more than their assets allow!
                if d.assets > claims_received_from_a:
                    # self.Insurer.gain(claims_received_from_a)
                    self.insurer_recoup(claims_received_from_a)
                    self.defender_lose(d, claims_received_from_a)
                    d.claims_received[a.id] -= claims_received_from_a
                else:
                    # self.Insurer.gain(d.assets)
                    self.insurer_recoup(claims_received_from_a)
                    self.defender_lose(d, d.assets)
                    d.claims_received[a.id] -= d.assets
                    # TODO remove d from living Defenders
            else:
                # All goes back to the Insurer
                # self.Insurer.gain(recoup)
                # print("      all goes back to insurer")
                self.insurer_recoup(recoup)
                d.claims_received[a.id] -= recoup
        else:
            self.defender_gain(d, gain=recoup)


    def insurer_lose(self, i, loss):
        i.lose(loss)
        self.paid_claims += loss
        assert i.assets + 1 >= 0, f'{self.params},'
        # print("insurer losing ", loss)

    def insurer_covers_d_for_losses_from_a(self, a, d, claim):
        # The defender gets to recoup losses from Insurer
        claims_amount = claim * self.params["CLAIMS"]
        if (claims_amount > self.Insurer.assets):
            # Insurer goes bust
            claims_amount = self.Insurer.assets
            self.insurer_time_of_death = self.iter_num
                    
        self.defender_gain(d, claims_amount) 
        # TODO make a separate handle claims function
        if self.a_i in d.claims_received:
            d.claims_received[a.id] += claims_amount
        else:
            d.claims_received[a.id] = claims_amount
        self.insurer_lose(self.Insurer, claims_amount)

    def insurer_recoup(self, recoup):
        self.Insurer.gain(recoup)
        self.paid_claims -= recoup
        # print("insurer recouping ", recoup)

    def government_gain(self, amount):
        # print(" -- gov gaining ", amount)
        self.Government.gain(amount)


    # TODO problem is that Defenders are getting losses recovered but also get to keep insurance claims...
    def a_distributes_loot(self, a):
        self.caught += 1
        # print("    distributing ", a.assets)
        # Distribute the loot to victims
        for (k,v) in a.victims.items():
            # Payback for as long as possible
            # print("  --",k,v)
            if a.assets > 0:
                
                if self.Defenders[k].assets == 0:
                    print("reviving the dead")
                    self.alive_defenders.insert(k)

                if a.assets > v:
                    # print("  full payback")
                    self.defender_recoup(a=a, d=self.Defenders[k], recoup=v)
                    self.attacker_lose(a, v)
                else:
                    # print("  partial payback")
                    self.defender_recoup(a=a, d=self.Defenders[k], recoup=a.assets)
                    self.attacker_lose(a, a.assets)
                    break
            else:
                break

        # Anything remaining goes to the Government
        if (a.assets != 0):
            # print("  government getting ", a.assets)
            self.government_gain(a.assets)
            self.attacker_lose(a, a.assets)
        
        # print("============")

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

    def fight(self, a, d):

        effective_loot = d.assets * self.params["PAYOFF"]
        # TODO maybe mercy kill the Defenders if the loot is very low?
        cost_of_attack = d.costToAttack
        expected_earnings = effective_loot * d.ProbOfAttackSuccess

        # TODO consider chance of getting caught in the attacker's decisionmaking
        # TODO TODO TODO

        if (expected_earnings > cost_of_attack) and (cost_of_attack < a.assets):
            # Attacker decides that it's worth it to attack
            self.attacks += 1

            # Pay the cost of attacking
            self.attacker_lose(a, cost_of_attack)
            self.attacker_expenditures += cost_of_attack
     
            # The attacker might get caught
            # TODO Maybe make this a function of the amount of tax collected (bigger gov = better at catching the criminals)
            if (np.random.uniform(0,1) < self.params["CAUGHT"]):    
                # print(f'Attacker[{a.id}] caught! Has {self.Attackers[a.id].assets} to distribute')
                self.a_distributes_loot(a)
            else:
                AttackerWins = (np.random.uniform(0,1) < d.ProbOfAttackSuccess)
                if (AttackerWins):
                    self.a_steals_from_d(a=a, d=d, loot=effective_loot)
                    
                    # Note: we do not re-scale a defender's costToAttack to be proportionate to the new level of assets
                    # This is because we assume previous security investments are still valid!

                    # Recoup losses from Insurer (if Insurer is not dead yet)
                    if self.Insurer.assets > 0:
                        self.insurer_covers_d_for_losses_from_a(a=a, d=d, claim=effective_loot)
                        
        # print(f'2: defender_iter_sum: {self.defender_iter_sum}')
  
    def run_iterations(self):

        # Lists of inidices of which players are still alive
        self.alive_attackers = set(range(len(self.Attackers)))
        self.alive_defenders = set(range(len(self.Defenders)))
        # print(self.alive_attackers)

        defenders_have_more_than_attackers = True

        for self.iter_num in range(1, self.game_settings['SIM_ITERS']+1):
            # print(">>>>>>>> ", self.iter_num, "<<<<<<<<<< current_defender_sum_assets=", self.current_defender_sum_assets)
            
            self.defender_iter_sum = 0
            self.attacker_iter_sum = 0

            dead_attackers = []
            dead_defenders = []

            # Make the pairings between Attackers and Defenders random
            # Have to make alive_attackers|defenders a list though in order to shuffle
            alive_attackers_list = list(self.alive_attackers)
            alive_defenders_list = list(self.alive_defenders)

            # Only shuffle the *longer* list
            # So that all players in the longer list get an equal chance to fight
            if len(alive_defenders_list) < len(alive_attackers_list):
                random.shuffle(alive_attackers_list)
            else:
                random.shuffle(alive_defenders_list)

            for di, ai in zip(alive_defenders_list, alive_attackers_list):
                self.fight(a=self.Attackers[ai], d=self.Defenders[di])
                if self.Attackers[ai].assets == 0:
                    dead_attackers.append(ai) 
                if self.Defenders[di].assets == 0:
                    dead_defenders.append(di)

            # Remove the dead players from the game
            for x in dead_attackers:
                self.alive_attackers.remove(x)
            for x in dead_defenders:
                self.alive_defenders.remove(x)
            
            # print(f'4: defender_iter_sum: {self.defender_iter_sum}')

            self.current_defender_sum_assets += self.defender_iter_sum
            self.current_attacker_sum_assets += self.attacker_iter_sum
            # assert abs((self.Insurer.assets + self.current_defender_sum_assets) - (self.current_attacker_sum_assets + self.attacker_expenditures)) < 1, f'{self.params}, {self.Insurer.assets}, {self.current_defender_sum_assets}, {self.current_attacker_sum_assets}, {self.attacker_expenditures}'
            assert self.d_init + 1 > self.current_defender_sum_assets, f'{self.params}, d_init={self.d_init}, current_defender_sum_assets={self.current_defender_sum_assets}'
            
            # Master checksum
            self.verify_state() # TODO remove later! Just for debugging



            # Check if there has been a crossover point
            if defenders_have_more_than_attackers:
                if (self.current_attacker_sum_assets > self.current_defender_sum_assets):
                    self.crossovers.append(self.iter_num)
                    defenders_have_more_than_attackers = False
            else:
                if (self.current_attacker_sum_assets < self.current_defender_sum_assets):
                    self.crossovers.append(self.iter_num)
                    defenders_have_more_than_attackers = True
            
            # Check if the game needs to be ended

            # Condition #1: Either the Defenders or the Attackers completely die off
            if len(self.alive_attackers) == 0:
                self.conclude_game('A')
                return
            
            if len(self.alive_defenders) == 0:
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
