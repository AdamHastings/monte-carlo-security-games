#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys, os
import glob
import pandas as pd

def make_df():
    path = r'../logs'
    files = glob.glob(path + "/*.csv")

    frames = []

    for f in files:
        df = pd.read_csv(f, index_col=False, header=0)
        frames.append(df)

    df = pd.concat(frames, axis=0, ignore_index=True)
    df.to_csv('../data/df.csv', index=False)


if __name__=="__main__":
    make_df()
            
