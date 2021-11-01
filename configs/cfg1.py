# Blue team are the defenders
# Red  team are the attackers
import numpy as np


# Parameters:
# 1. PERCENT_EVIL (percentage of players that are adversaries)
# 2. WEALTH_GAP (mean difference between two distributions)
# 3. PAYOFF (percentage of wealth transferred after a sucessful attack)

# Mandate essentially reduces the payoff

# TODO: Simplify so that game_params contains these three (and only these three) params

# Parameters of the game itself
params = dict(
    PERCENT_EVIL = 0.01,
    PAYOFF = 0.05,
    WEALTH_GAP = 0.2
)

game_settings = dict(
    TOTAL_PLAYERS = 1000,
    SIM_ITERS = 20000,
    NUM_GAMES = 9,
    MANDATE = False,
    SEC_INVESTMENT = 0.1
)

# skew of 0 is a normal distribution
blue = dict(
    dist = "lognormal",
    mu = 0,
    sigma = 10,
    scale = 1,
    EARNINGS = 0,
    ASSETS = 100000
)

red = dict(
    dist = "lognormal",
    mu = 0,
    sigma = 1,
    scale = 1,
    ASSETS = 10000
)

