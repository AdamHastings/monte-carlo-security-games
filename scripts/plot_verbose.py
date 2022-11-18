import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import json
import os, sys

try:
    if len(sys.argv) != 2:
        raise Exception("\nERROR: Incorrect number of args!")
    f = '../logs/' + sys.argv[1]
    df = pd.read_csv(f, index_col=False, header=0)
except Exception as e:
    print(e)
    print("\nExample of how to run config test_cfg1")
    print("\n     $ python3 plot_verbose.py test_target.csv\n\n")
    sys.exit(0)

def makeArray(text): 
    text = text[1:-1]
    ret = np.fromstring(text, sep=", ")
    return ret

def createTitle(df, i):
    ret = ""
    # ret += "MANDATE=" + str(df["MANDATE"][i]) + "\n\n"
    ret += "CAUGHT=" + str(df["CAUGHT"][i]) + ", "
    ret += "CLAIMS=" + str(df["CLAIMS"][i]) + ", "
    ret += "EFFICIENCY=" + str(df["EFFICIENCY"][i]) + "\n"
    ret += "EFFORT=" + str(df["EFFORT"][i]) + ", "
    ret += "INEQUALITY=" + str(df["INEQUALITY"][i]) + ", "
    ret += "ATTACKERS=" + str(df["ATTACKERS"][i]) + "\n"
    ret += "PAYOFF=" + str(df["PAYOFF"][i]) + ", "
    ret += "PREMIUM=" + str(df["PREMIUM"][i]) + ", "
    ret += "TAX=" + str(df["TAX"][i]) + "\n\n"
    return ret





df['defenders_cumulative_assets'] = df['defenders_cumulative_assets'].apply(makeArray)
df['attackers_cumulative_assets'] = df['attackers_cumulative_assets'].apply(makeArray)
df['insurer_cumulative_assets'] = df['insurer_cumulative_assets'].apply(makeArray)
df['government_cumulative_assets'] = df['government_cumulative_assets'].apply(makeArray)

df = df.sort_values(by=['MANDATE'],ignore_index=True)

# sys.exit(0)

# fig = plt.figure(figsize=(4,3))

fig, axs = plt.subplots(5,2, figsize=(6, 9))

for i in range(df.shape[0]):
    x = np.arange(df.final_iter[i] + 1)
    axs[i//2, i%2].plot(x, df['defenders_cumulative_assets'][i], label="Defenders", linestyle="-", color="blue")
    axs[i//2, i%2].plot(x, df['attackers_cumulative_assets'][i], label="Attackers", linestyle=":", color="red")
    axs[i//2, i%2].plot(x, df['insurer_cumulative_assets'][i], label="Insurer", linestyle="--", color="orange")
    axs[i//2, i%2].plot(x, df['government_cumulative_assets'][i], label="Government", linestyle="-.", color="green")
    axs[i//2, i%2].get_xaxis().set_ticks([])
    axs[i//2, i%2].get_yaxis().set_ticks([])
    axs[i//2, i%2].set_title("MANDATE=" + str(df['MANDATE'][i]))
    axs[i//2, i%2].set_ylim(0,1.2*max(max(df['defenders_cumulative_assets'][0]), max(df['attackers_cumulative_assets'][0]), max(df['insurer_cumulative_assets'][0]), max(df['government_cumulative_assets'][0])))
    axs[i//2, i%2].set_xlim(0,df['final_iter'][i])
    
    # plt.title(title)

plt.figlegend(axs[0,0].get_lines(), ["Defenders", "Attackers", "Insurer", "Government"], loc = 'lower center', fancybox=True, shadow=True, ncol=4, bbox_to_anchor=[0.5, -0.05])
    
for ax in axs.flat:
    ax.set(xlabel='Time', ylabel='Assets')


title = createTitle(df, 0)
fig.suptitle(title)
title = title.replace(" ", "")
title = title.replace(",", "_")
title = title.replace("\n", "_")    
filename = "figures/" + sys.argv[1][:-4] + "_" + title
plt.tight_layout()
plt.savefig(filename + ".png", bbox_inches='tight')



