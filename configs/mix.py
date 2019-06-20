game = dict(
    BLUETEAM_SIZE = 500,
    REDTEAM_SIZE = 10,
    SIM_ITERS = 10000,
    COST_TO_ATTACK = 500,
    LOOT = 2500,
    NUM_GAMES = 4,
)

blue = dict(
    dist = "skew",
    a = 0,
    scale = 10000,
)

red = dict(
    dist = "lognormal",
    mu = 5,
    sigma = 0.6,
    scale = 10000,
)
