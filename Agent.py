import random

class Agent:
    def __init__(self, assets, skill):
        self.assets = None
        self.skill = None

    def get_assets(self):
        return self.assets

class Defender(Agent):
    def __init__(self, assets, skill):
        Agent.__init__(self, assets, skill)
        self.assets = assets
        self.skill = skill

    def lose(self, loot):
        # if (loot > self.assets):
        #     amount_lost = self.assets
        #     self.assets = 0
        #     return amount_lost
        # else:
            amount_lost = loot
            self.assets -= amount_lost

            # invest in security and get better
            # self.money -= 20
            # self.protection *= 1.2
            
            return amount_lost

   

class Attacker(Agent):
    def __init__(self, assets, skill):
        Agent.__init__(self, assets, skill)
        self.assets = assets
        self.skill = skill

    def win(self, loot, cost):
        self.assets = self.assets + loot - cost
    
    def lose(self, cost):
        self.assets -= cost 

        # self.money -= 30
        # self.offenselevel *= 1.1