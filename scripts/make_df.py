#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys, os
import glob
import pandas as pd

path = r'../logs'
files = glob.glob(path + "/*.csv")

frames = []

for f in files:
    df = pd.read_csv(f, index_col=None, header=0)
    frames.append(df)

df = pd.concat(frames, axis=0, ignore_index=True)
df.to_csv('../data/df.csv', index=False)

        
