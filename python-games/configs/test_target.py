import numpy as np
import sys

"""Range of reasonable params

Format: (start, end, increment)
"""

params_ranges = dict(
    ATTACKERS_range  = [0.1], # Percentage of Attackers, relative to BLUE_PLAYERS
    CAUGHT_range     = [1.0], # Probability that an attacker is caught and has their assets confiscated
    CLAIMS_range     = [0.1], # Percentage of losses paid back to defender by Insurer
    EFFICIENCY_range = [0.1], # Percent of MANDATE that goes towards increasing a defender's costToAttack
    EFFORT_range     = [0.55], # Percentage of a defender's assets that must be spent by an Attacker to attempt an attack
    INEQUALITY_range = [0.1], # How much poorer Attackers are than Defenders, as a pct
    MANDATE_range    = [0.9], # Percentage of assets that are spent on security measures.
    PAYOFF_range     = [1.0], # How much of a defender's assets are stolen if attacker is successful
    PREMIUM_range    = [0.5], # Percentage of MANDATE that is allocated towards security
    TAX_range        = [0.5], # Percentage of confiscated assets that are retained by Government
)

game_settings = dict(
    BLUE_PLAYERS = 1000,
    SIM_ITERS = 10000,
    NUM_GAMES = 1,
    EPSILON_DOLLARS = 100,
    DELTA_ITERS = 50
)

verbose=True

# Find the params that are pinned to a single value 
# (and will be run on a single machine)
_pinned_vals = {}
for (k,v) in params_ranges.items():
        if len(v) == 1:
            _pinned_vals[k] = v

# set global filename to be used by all worker threads
_cfg_name = ".".join(__file__.split('/')[-1].split('.')[:-1])

LOGFILE = "logs/" + _cfg_name + ".csv"
