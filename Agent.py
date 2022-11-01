import random
from numpy.random import choice
import numpy as np


'''
Base class for all agents
'''
class Agent:
    def __init__(self, assets):
        self.assets = assets

    def get_assets(self):
        return self.assets


class Defender(Agent):
    def __init__(self, assets, ProbOfAttackSuccess, costToAttack):
        Agent.__init__(self, assets)
        self.ProbOfAttackSuccess = ProbOfAttackSuccess
        self.costToAttack = costToAttack
        self.insurance = 0

    def lose(self, loot):
        self.assets -= loot

    def buy_insurance(self, premium):
        self.assets -= premium


    def gain(self, gain):
        self.assets += gain

class Attacker(Agent):
    def __init__(self, assets):
        Agent.__init__(self, assets)

    def win(self, loot, cost):
        self.assets = self.assets + loot - cost
    
    def lose(self, cost):
        self.assets -= cost 

# TODO: One shot insurance, or continuous investments?
#    A: We will do one-shot for now
class Insurer(Agent):
    def __init__(self, assets):
        Agent.__init__(self, assets)

    def collect_premium(self, premiums):
        self.assets += premiums

    def remit_claim(self, claim):
        self.assets -= claim
        return claim

class Government(Agent):
    def __init__(self, assets):
        Agent.__init__(self, assets)

    def confiscate(self, loot):
        self.assets += loot

'''
Functions to create Agents
'''
def create_blue_agent(SUCCESS):

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
    costToAttack = assets * SUCCESS # TODO discuss later if we want to change this derivation

    return Defender(assets, ProbOfAttackSuccess, costToAttack)

def create_red_agent(INEQUALITY):
    randwealth = [random.randint(0,9999), random.randint(10000,99999), random.randint(100000,999999), random.randint(1000000,9999999)]        
    assets = choice(randwealth, 1, p=[0.55, 0.33, 0.11, 0.01])[0] * INEQUALITY
    return Attacker(assets)

def create_insurer(Defenders, PREMIUM):
    
    ins = Insurer(0)
    # Initialize Insurer with assets
    for d in Defenders:
        premium = d.assets * PREMIUM
        d.buy_insurance(premium)
        ins.collect_premium(premium)

    return ins