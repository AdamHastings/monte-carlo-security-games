# Blue team are the defenders
# Red  team are the attackers
import numpy as np


# Parameters of the game itself
game = dict(
    BLUETEAM_SIZE = 1000,
    REDTEAM_SIZE = 100,
    SIM_ITERS = 1000,
    COST_TO_ATTACK = 950,
    LOOT = 2000,
    NUM_GAMES =4,
)

# skew of 0 is a normal distribution
blue = dict(
    dist = "normal",
    mu = 0,
    sigma = 1,
    scale = 1,
    EARNINGS = 150,
    ASSETS = 100000
)

red = dict(
    dist = "normal",
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
