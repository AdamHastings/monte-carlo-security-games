import numpy as np
import sys

"""Range of reasonable params

Format: (start, end, increment)
"""

params_ranges = dict(
    # Parameters that affect game initialization
    MANDATE_range    = np.linspace(0.0, 0.9, 2), # Percentage of assets that are spent on security measures.
    ATTACKERS_range  = np.linspace(0.1, 1.0, 2), # Percentage of Attackers, relative to BLUE_PLAYERS
    INEQUALITY_range = np.linspace(0.1, 1.0, 2), # How much poorer Attackers are than Defenders, as a pct
    PREMIUM_range    = np.linspace(0.0, 1.0, 2), # Percentage of MANDATE that is allocated towards security
    EFFICIENCY_range = np.linspace(0.1, 1.0, 2), # Percent of MANDATE that goes towards increasing a defender's costToAttack
    SUCCESS_range    = np.linspace(0.1, 1.0, 2), # Percentage of a defender's assets that must be spent by an Attacker to attempt an attack

    # Parameters that affect gameplay
    PAYOFF_range = np.linspace(0.1, 1.0, 2), # How much of a defender's assets are stolen if attacker is successful
    CAUGHT_range = np.linspace(0.0, 1.0, 2), # Probability that an attacker is caught and has their assets confiscated
    CLAIMS_range = np.linspace(0.1, 1.0, 2), # Percentage of losses paid back to defender by Insurer
    TAX_range    = np.linspace(0.1, 0.2, 2), # Percentage of confiscated assets that are retained by Government
)

game_settings = dict(
    BLUE_PLAYERS = 2000,
    SIM_ITERS = 10000,
    NUM_GAMES = 1,
    EPSILON_DOLLARS = 100,
    DELTA_ITERS = 100
)

# Find the params that are pinned to a single value 
# (and will be run on a single machine)
_pinned_vals = {}
for (k,v) in params_ranges.items():
        if len(v) == 1:
            _pinned_vals[k] = v

# set global filename to be used by all worker threads
_cfg_name = __file__.split('/')[-1].split('.')[0]

if (len(_pinned_vals) > 0):
    _pinned_name = '_'.join([str(k) + "=" + str(v[0]) for k,v in _pinned_vals.items()])
else:
    _pinned_name = "all"

LOGFILE = "logs/" + _cfg_name + '_' + _pinned_name + ".csv"
