import random

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

    def lose(self, loot):
            amount_lost = loot
            self.assets -= amount_lost
            
            return amount_lost
    
    def recoup(self, gain):
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

    def collect_premiums(self, premiums):
        self.assets += premiums

    def remit_claim(self, claim):
        self.assets -= claim
        return claim
