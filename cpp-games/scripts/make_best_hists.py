#!/usr/bin/python3

import pandas as pd

df=pd.read_csv('parsed.csv')

df = df[['MANDATE', 'TAX', 'PREMIUM', 'result']]

df = df.sort_values(by=['MANDATE', 'TAX', 'PREMIUM'])

print(df)

df.to_csv('rearranged.csv', index=False)