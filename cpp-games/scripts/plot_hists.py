#!/usr/bin/python3
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

df = pd.read_csv("make_hists_log.txt", index_col=0, header=0)

print(df)

plt.figure(figsize=(4, 3))
# plt.title("Useful Monte-Carlo simulation parameter distributions")
plt.xlabel("Parameter values")
# plt.xticks(si_nums)
plt.ylabel("Number of games")
plt.grid()
plt.xlim(0,1)
# plt.ylim(0, 50000)

for index, row in df.iterrows():
    # print(index, row)
    print(index)
    print(row)

    xvals = row.to_numpy()
    # xvals = row.loc[df[index] != "x"]
    # yvals = 
    # xvals = df.loc[~(df==0).all(axis=1)]
    # xvals = df

    print(xvals)

