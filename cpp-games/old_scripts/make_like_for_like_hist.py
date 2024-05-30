#!/usr/bin/python3

import pandas as pd
import matplotlib.pyplot as plt

df=pd.read_csv('parsed.csv')

df = df[['MANDATE', 'TAX', 'PREMIUM', 'result']]

df = df.sort_values(by=['MANDATE', 'TAX', 'PREMIUM'])

df.to_csv('rearranged.csv', index=False)

checksum = df['result'].sum()
print(checksum)
expected = (10 * 10 * 10 * 10 * 10 * 10) * (11 * 11 * 11)
assert checksum == expected, 'checksum failed: found ' + str(checksum) + ', expected ' + str(expected)

hists = [df.loc[df['MANDATE'] == x]['result'].sum() for x in df['MANDATE'].unique()]

print(hists)

mands = df['MANDATE'].unique()
xmandticks = [str(x) + "0%" for x in mands ]

assert sum(hists) == expected

plt.bar(mands, hists)
plt.xticks(mands, xmandticks)
plt.xlabel('MANDATE')
plt.ylabel('count')
# plt.show()
plt.savefig('figures/like-for-like.png')