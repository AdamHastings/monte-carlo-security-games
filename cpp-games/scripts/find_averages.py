#!/usr/bin/python3


# MANDATE_range = [0.0, 0.1, 0.2, 0.3, 0.4, 0.5]
# PREMIUM_range = [0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0]
# TAX_range     = [0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0]

# # MANDATE_range = ['0.0', '0.1', '0.2', '0.3', '0.4', '0.5']
# # PREMIUM_range = ['0.0', '0.1', '0.2', '0.3', '0.4', '0.5', '0.6', '0.7', '0.8', '0.9', '1.0']
# # TAX_range     = ['0.0', '0.1', '0.2', '0.3', '0.4', '0.5', '0.6', '0.7', '0.8', '0.9', '1.0']


# for m in MANDATE_range:
#     for p in PREMIUM_range:
#         for t in TAX_range:
#             filename = "../trimmed/MANDATE=" + str(m) + "_PREMIUM=" + str(p) + "_TAX=" + str(t) + ".csv"
            
#             with open(filename) as infile:
#                 # toss the firstline
#                 line = infile.readline()
#                 print(line)
#                 while line:
#                     line = infile.readline()

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

df = pd.read_csv("make_hists_log.txt", index_col=0, header=0)
print(df)

row_sum = df.sum(axis=1)
print(row_sum)
# assert row_sum.nunique().iloc[0] == 1, "err: mismatched row sums"
# assert (row_sum.sum(axis=1) == row_sum.sum(axis=1)[0]).all()



print("\nmode:")
mode = df.idxmax(axis=1)
print(mode)
print("")

print("\nmean:")
cols = df.columns.to_numpy(dtype=float)
mean = df.apply(lambda row: np.nansum(row * cols) / row.sum(), axis=1).round(1)
print(mean)

print("median:")


# plt.figure(figsize=(4, 3))
# plt.title("Useful Monte-Carlo simulation parameter distributions")
# plt.xlabel("Parameter values")
# # ticks = 
# # print(ticks)
# # plt.xticks(list(map(float, df.columns)))
# plt.ylabel("Number of games")
# # plt.grid()
# # plt.ylim(0, 50000)

# for index, row in df.iterrows():
#     tmp = row.dropna()
#     xvals = list(map(float, tmp.index))
#     yvals = tmp.tolist()
    
#     plt.plot(xvals, yvals, label=index)

# plt.legend(loc='best',title="Game parameter:", bbox_to_anchor=(1.0, 0.6, 0.3, 0.5), fancybox=True, shadow=True, ncol=1)
# plt.tight_layout()
# plt.savefig("figures/hist_mc.pdf")
# plt.savefig("figures/hist_mc.png")

