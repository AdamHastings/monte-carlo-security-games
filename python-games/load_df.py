import pandas as pd

f = 'logs/test_cfg1_all.csv'
df = pd.read_csv(f, index_col=False, header=0)
return df