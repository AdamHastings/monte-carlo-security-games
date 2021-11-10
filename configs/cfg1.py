# Blue team are the defenders
# Red  team are the attackers
import numpy as np


# Parameters:
# 1. PERCENT_EVIL (percentage of players that are adversaries)
# 2. WEALTH_GAP (mean difference between two distributions)
# 3. PAYOFF (percentage of wealth transferred after a successful attack)

# params = dict(
#     PERCENT_EVIL = 0.01, # almost seems irrelevant: simply determines how fast the system converges
#     PAYOFF = 0.2,
#     WEALTH_GAP = 0.1,
#     SEC_INVESTMENT_CONVERSION_RATE = 0.2,
#     ATTACK_COST_CONVERSION_RATE = 0.01,
#     SEC_INVESTMENT = 0.1,
#     CHANCE_OF_GETTING_CAUGHT = 0.005
# )

"""Range of reasonable params

Format: (start, end, increment)
"""
params_ranges = dict(
    PERCENT_EVIL = np.linspace(0.3, 0.4, 2),
    PAYOFF = np.linspace(0.8, 0.9, 2),
    WEALTH_GAP = np.linspace(0.4, 0.8, 2),
    SEC_INVESTMENT_CONVERSION_RATE = np.linspace(0.2, 0.3, 2),
    ATTACK_COST_CONVERSION_RATE = np.linspace(0.1, 0.2, 2),
    CHANCE_OF_GETTING_CAUGHT = np.linspace(0.01, 0.02, 2),
    SEC_INVESTMENT = np.linspace(0.1, 0.2, 2)
)

game_settings = dict(
    BLUE_PLAYERS = 2000,
    SIM_ITERS = 1000,
    NUM_GAMES = 1,
    EPSILON_DOLLARS = 100,
    STABLE_ITERS = 50
)

