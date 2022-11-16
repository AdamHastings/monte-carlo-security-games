import numpy as np
import sys

"""Range of reasonable params

Format: (start, end, increment)
"""

params_ranges = dict(
    # Parameters that affect game initialization
    MANDATE_range    = [0.7                         ], # Percentage of assets that are spent on security measures.
    ATTACKERS_range  = [     0.2, 0.4, 0.6, 0.8, 1.0], # Percentage of Attackers, relative to BLUE_PLAYERS
    INEQUALITY_range = [     0.2, 0.4, 0.6, 0.8, 1.0], # How much poorer Attackers are than Defenders, as a pct
    PREMIUM_range    = [0.0, 0.2, 0.4, 0.6, 0.8, 1.0], # Percentage of MANDATE that is allocated towards insurance
    EFFICIENCY_range = [     0.2, 0.4, 0.6, 0.8, 1.0], # Percent of MANDATE that goes towards increasing a defender's costToAttack
    EFFORT_range     = [     0.2, 0.4, 0.6, 0.8, 1.0], # Percentage of a defender's assets that must be spent by an Attacker to attempt an attack
    PAYOFF_range     = [     0.2, 0.4, 0.6, 0.8, 1.0], # How much of a defender's assets are stolen if attacker is successful
    CAUGHT_range     = [0.0, 0.2, 0.4, 0.6, 0.8     ], # Probability that an attacker is caught and has their assets confiscated
    CLAIMS_range     = [     0.2, 0.4, 0.6, 0.8, 1.0], # Percentage of losses paid back to defender by Insurer
    TAX_range        = [0.0, 0.2, 0.4, 0.6, 0.8, 1.0], # Percentage of selfless MANDATE is sent to the government (to increase prosecution)
    # Parameters that affect gameplay
)

game_settings = dict(
    BLUE_PLAYERS = 1000,
    SIM_ITERS = 10000,
    NUM_GAMES = 1,
    EPSILON_DOLLARS = 100,
    DELTA_ITERS = 50
)

# set global filename to be used by all worker threads
_cfg_name = __file__.split('/')[-1].split('.')[0]

LOGFILE = "logs/" + _cfg_name + ".csv"
