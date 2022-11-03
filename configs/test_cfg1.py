# Blue team are the defenders
# Red  team are the attackers
import numpy as np

# Parameters:
# 1. PERCENT_EVIL (percentage of players that are adversaries)
# 2. WEALTH_GAP (mean difference between two distributions)
# 3. PAYOFF (percentage of wealth transferred after a successful attack)

"""Range of reasonable params

Format: (start, end, increment)
"""
params = dict(
    ATTACKERS = np.linspace(0.1, 1.0, 2), # Percentage of Attackers, relative to BLUE_PLAYERS
    PAYOFF = np.linspace(0.1, 1.0, 2), # How much of a defender's assets are stolen if attacker is succesful
    INEQUALITY = np.linspace(0.1, 1.0, 2), # How much poorer Attackers are than Defenders, as a pct
    EFFICIENCY = np.linspace(0.1, 1.0, 2), # Percent of MANDATE that goes towards increasing a defender's costToAttack
    SUCCESS = np.linspace(0.1, 1.0, 2), # Percentage of a defender's assets that must be spent by an Attacker to attempt an attack
    CAUGHT = np.linspace(0.0, 0.0, 2), # Probability that an attacker is caught and has their assets confiscated
    CLAIMS = np.linspace(0.1, 1.0, 2), # Percentage of losses paid back to defender by Insurer
    PREMIUMS = np.linspace(0.1, 1.0, 2), # Percentage of MANDATE that is allocated towards security
    TAX = np.linspace(0.1, 1.0, 0.1,) # Percentage of confiscated assets that are retained by Government
    MANDATE = np.linspace(0.0, 1.0, 1) # Percentage of assets that are spent on security measures.
)

game_settings = dict(
    BLUE_PLAYERS = 2000,
    SIM_ITERS = 10000,
    NUM_GAMES = 1,
    EPSILON_DOLLARS = 100,
    STABLE_ITERS = 50
)

# Find the params that are pinned to a single value 
# (and will be run on a single machine)
_pinned_vals = {}
for (k,v) in params.items():
        if len(v) == 1:
            _pinned_vals[k] = v

# set global filename to be used by all worker threads
FILENAME = "logs/stats_" + '_'.join([str(k) + "=" + str(v[0]) for k,v in _pinned_vals.items()]) + ".csv"


