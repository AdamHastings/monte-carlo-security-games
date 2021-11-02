# Blue team are the defenders
# Red  team are the attackers
import numpy as np


# Parameters:
# 1. PERCENT_EVIL (percentage of players that are adversaries)
# 2. WEALTH_GAP (mean difference between two distributions)
# 3. PAYOFF (percentage of wealth transferred after a sucessful attack)

params = dict(
    PERCENT_EVIL = 0.37,
    PAYOFF = 0.1,
    WEALTH_GAP = 0.2,
    SEC_INVESTMENT_CONVERSION_RATE = 0.1,
    ATTACK_COST_CONVERSION_RATE = 0.01,
    SEC_INVESTMENT = 0.1
)

game_settings = dict(
    BLUE_PLAYERS = 1000,
    SIM_ITERS = 500,
    NUM_GAMES = 1,
)

