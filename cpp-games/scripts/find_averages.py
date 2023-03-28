#!/usr/bin/python3


import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

df = pd.read_csv("make_hists_log.txt", index_col=0, header=0)
print(df)

row_sum = df.sum(axis=1)
assert row_sum.nunique() == 1

print("\nmode:")
mode = df.idxmax(axis=1)
print(mode)
print("")

print("\nmean:")
cols = df.columns.to_numpy(dtype=float)
mean = df.apply(lambda row: np.nansum(row * cols) / row.sum(), axis=1).round(1)
print(mean)

def get_median(row, len):
    midpoint = len // 2
    count = 0 # count vals until we reach midpoint
    for val, col in zip(row.tolist(), row.index):
        if (np.isnan(val)):
            continue
        else:
            count += val
            if count > midpoint:
                return col


print("\nmedian:")
median = df.apply(lambda row: get_median(row, row.sum()), axis=1)
print(median)



