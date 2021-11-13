#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
import pandas as pd
from termcolor import colored
from make_df import make_df
from make_plots import make_plots


parser = argparse.ArgumentParser()
parser.add_argument('-r', action='store_true', help='rebuild dataframe from ../logs/*.csv')
# parser.add_argument('-v', action='store_true', help='verify that the uploads pass all sanity/integrity checks')
args = parser.parse_args()

if(args.r):
    print(colored('[+] Making DataFrame', 'green'))
    make_df()

print(colored('[+] Loading DataFrame', 'green'))
df = pd.read_csv('../data/df.csv', index_col=None, header=0)
print(df.shape)
print(colored('[+] Creating plots', 'green'))
make_plots(df)

    