# Blue team are the defenders
# Red  team are the attackers
import numpy as np


# Parameters:
# 1. PERCENT_EVIL (percentage of players that are adversaries)
# 2. WEALTH_GAP (mean difference between two distributions)
# 3. PAYOFF (percentage of wealth transferred after a sucessful attack)

params = dict(
    PERCENT_EVIL = 0.5, # almost seems irrelevant: simply determines how fast the system converges
    PAYOFF = 0.1,
    WEALTH_GAP = 0.1,
    SEC_INVESTMENT_CONVERSION_RATE = 0.1,
    ATTACK_COST_CONVERSION_RATE = 0.01,
    SEC_INVESTMENT = 0.1
)

game_settings = dict(
    BLUE_PLAYERS = 2000,
    SIM_ITERS = 1000,
    NUM_GAMES = 1,
)

