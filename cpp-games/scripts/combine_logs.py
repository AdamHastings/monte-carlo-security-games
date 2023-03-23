import pandas as pd

# Create two dataframes
df1 = pd.read_csv("make_hists_log.txt", index_col=0, header=0)
df2 = pd.read_csv("make_hists_log_TAX=0.0-0.3_except_TAX=0.3_PREMIUM=1.0.txt", index_col=0, header=0)
df3 = pd.read_csv("make_hist_log_TAX=0.4_and_up.txt", index_col=0, header=0)

# Concatenate the two dataframes
result = df1.add(df2)
result = result.add(df3)

# Print the result
print(result)

result.to_csv("combined.csv")