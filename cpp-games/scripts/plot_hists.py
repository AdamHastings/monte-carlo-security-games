#!/usr/bin/python3
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

df = pd.read_csv("make_hists_log.txt", index_col=0, header=0)

print(df)

plt.figure(figsize=(4, 3))
# plt.title("Useful Monte-Carlo simulation parameter distributions")
plt.xlabel("Parameter values")
# ticks = 
# print(ticks)
# plt.xticks(list(map(float, df.columns)))
plt.ylabel("Number of games")
# plt.grid()
# plt.ylim(0, 50000)

for index, row in df.iterrows():
    tmp = row.dropna()
    xvals = list(map(float, tmp.index))
    yvals = tmp.tolist()
    
    plt.plot(xvals, yvals, label=index)

plt.legend(loc='best',title="Game parameter:", bbox_to_anchor=(1.0, 0.6, 0.3, 0.5), fancybox=True, shadow=True, ncol=1)
plt.tight_layout()
plt.savefig("figures/hist_mc.pdf")
plt.savefig("figures/hist_mc.png")

