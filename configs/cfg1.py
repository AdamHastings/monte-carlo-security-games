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
game = dict(
    BLUETEAM_SIZE = 1000,
    REDTEAM_SIZE = 10,
    SIM_ITERS = 6000,
    COST_TO_ATTACK = 500,
    LOOT_PCT = 0.2,
    NUM_GAMES =1,
)

# skew of 0 is a normal distribution
blue = dict(
    dist = "lognormal",
    mu = 0,
    sigma = 1,
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

# blueteam = dict(
#     assets = np.full(game['BLUETEAM_SIZE'], 1000),
#     # assets = 1000,
#     skill = np.random.normal(size=game['BLUETEAM_SIZE'])
# )

# redteam = dict(
#     # assets = np.random.normal(game['BLUETEAM_SIZE'])
#     assets = np.full(game['REDTEAM_SIZE'], 200),
#     skill = np.random.normal(size=game['REDTEAM_SIZE'])
# )

# # print(redteam['skill'])
# # print(np.random.normal(size=100))

# red_agent = dict(
#     assets = 200,
#     skill = np.random.normal()
# )
