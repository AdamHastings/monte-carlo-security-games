#!/usr/bin/python3
import pandas as pd
import numpy as np
import argparse
import copy
import sys
from plot_cumulative_assets import plot_cumulative_assets
from asset_flow_sankey import asset_flow_sankey
from plot_canary_vars import plot_canary_vars, plot_p_attacks
from choices import choices


# default 
filename = "../logs/fullsize_short.csv"

if (len(sys.argv) == 2):
    filename = sys.argv[1]
elif (len(sys.argv) > 2):
    print("Too many arguments!")
    sys.exit(1)

df = pd.read_csv(filename, header=0)

print("plot_cumulative_assets")
plot_cumulative_assets(copy.deepcopy(df))

print("asset_flow_sankey")
asset_flow_sankey(copy.deepcopy(df))

print("plot_canary_vars")
plot_canary_vars(copy.deepcopy(df))

print("plot_p_attacks")
plot_p_attacks(copy.deepcopy(df))

print("choices")
choices(copy.deepcopy(df))