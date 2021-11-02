import random

class Agent:
    def __init__(self, assets):
        self.assets = assets

    def get_assets(self):
        return self.assets

class Defender(Agent):
    def __init__(self, assets, ProbOfAttackSuccess):
        Agent.__init__(self, assets)
        self.ProbOfAttackSuccess = ProbOfAttackSuccess

    def lose(self, loot):
            amount_lost = loot
            self.assets -= amount_lost
            
            return amount_lost

   

class Attacker(Agent):
    def __init__(self, assets):
        Agent.__init__(self, assets)

    def win(self, loot, cost):
        self.assets = self.assets + loot - cost
    
    def lose(self, cost):
        self.assets -= cost 