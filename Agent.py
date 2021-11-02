import random

class Agent:
    def __init__(self, assets):
        self.assets = assets

    def get_assets(self):
        return self.assets

class Defender(Agent):
    def __init__(self, assets, likelihood_of_attack):
        Agent.__init__(self, assets)
        self.likelihood_of_attack = likelihood_of_attack

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