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




def run_all(filename):
    df = pd.read_csv(filename, header=0)

    filename = filename.replace("../logs/", "")
    filename = filename.replace(".csv", "")
    df['folder'] = filename

    print("  plot_cumulative_assets")
    plot_cumulative_assets(copy.deepcopy(df))

    print("  asset_flow_sankey")
    asset_flow_sankey(copy.deepcopy(df))

    print("  plot_canary_vars")
    plot_canary_vars(copy.deepcopy(df))

    print("  plot_p_attacks")
    plot_p_attacks(copy.deepcopy(df))

    print("  choices")
    choices(copy.deepcopy(df))

if (len(sys.argv) == 2):
    filename = sys.argv[1]
    print(filename)
    run_all(filename)
else:
    # if no filename specified, run a set of defaults 
    defaults = [
        "../logs/fullsize_short.csv",
        "../logs/fullsize_short_mandatory_insurance.csv",
        "../logs/fullsize_short_selfless_insurers.csv",
        "../logs/fullsize_short_MANDATORY_INVESTMENT=0.00.csv",
        "../logs/fullsize_short_MANDATORY_INVESTMENT=0.01.csv",
        "../logs/fullsize_short_MANDATORY_INVESTMENT=0.02.csv",
        "../logs/fullsize_short_MANDATORY_INVESTMENT=0.03.csv",
        "../logs/fullsize_short_MANDATORY_INVESTMENT=0.04.csv",
        "../logs/fullsize_short_MANDATORY_INVESTMENT=0.05.csv",
    ]

    for f in defaults:
        print(f)
        run_all(f)
        print("")

