#!/usr/bin/python3
import pandas as pd
import numpy as np
import argparse
import sys
from plot_verbose import plot_verbose
from asset_flow_sankey import asset_flow_sankey



filename = "../logs/fullsize_short.csv"

if (len(sys.argv) == 2):
    filename = sys.argv[1]
elif (len(sys.argv) > 2):
    print("Too many arguments!")
    sys.exit(1)

df = pd.read_csv(filename, header=0)

print("plot_verbose")
plot_verbose(df)

print("asset_flow_sankey")
asset_flow_sankey(df)

