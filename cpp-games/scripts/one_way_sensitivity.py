#!/usr/bin/python3
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as mtick


params = ['MANDATE','ATTACKERS','INEQUALITY','PREMIUM','EFFICIENCY','EFFORT','PAYOFF','CAUGHT','CLAIMS','TAX']

df = pd.DataFrame()

for p in params:
    newdf = pd.read_csv("../logs/ows_" + p + ".csv")
    newdf['target'] = p
    df = pd.concat([df, newdf], ignore_index=True)


df['d_init_original'] = df['d_init'] / (1 - df['MANDATE'])

df['loss'] = 1 - df['d_end'] / df['d_init_original']
print(df[['MANDATE', 'd_init', 'd_init_original', 'd_end', 'loss']])

# print(df)
# df = df.groupby(params)['loss'].mean()

print(df)


fig, ax = plt.subplots()


for p in params:
    print(p)
    yvals = df.loc[df['target'] == p, 'loss'].values
    xvals = df.loc[df['target'] == p, p].values

    yvals_sorted = [y for _,y in sorted(zip(xvals, yvals))] 
    xvals_sorted = sorted(xvals)
    # print(xvals, yvals)
    # print(xvals_sorted, yvals_sorted)
    # print('----')
    ax.plot(xvals_sorted, yvals_sorted, label=p)


plt.xlabel('parameter')
plt.ylabel('loss')
ax.yaxis.set_major_formatter(mtick.PercentFormatter(1.0))

plt.legend(loc='best',title="Game parameter:", bbox_to_anchor=(1.0, 0.6, 0.3, 0.5), fancybox=True, shadow=True, ncol=1)
plt.tight_layout()
plt.savefig("figures/ows.png")
plt.savefig("figures/ows.pdf")