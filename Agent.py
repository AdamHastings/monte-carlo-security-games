import random
import numpy as np

class Agent:
    def __init__(self):
        self.money = 100

class Defender(Agent):
    def __init__(self):
        Agent.__init__(self)
        self.protection = np.random.normal()
        self.money = 1000

    def lose(self, loot):
        if (loot > self.money):
            amount_lost = self.money
            self.money = 0
            return amount_lost
        else:
            amount_lost = loot
        self.money -= amount_lost

        # invest in security and get better
        # self.money -= 20
        # self.protection *= 1.2

        return amount_lost

    def get_money(self):
        return self.money

class Attacker(Agent):
    def __init__(self):
        Agent.__init__(self)
        self.money = 200
        self.offenselevel = np.random.normal()

    def win(self, loot, cost):
        self.money = self.money + loot - cost
    
    def lose(self, cost):
        self.money -= cost 

        # self.money -= 30
        # self.offenselevel *= 1.1

    def get_money(self):
        return self.money