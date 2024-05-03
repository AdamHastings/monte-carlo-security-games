from sklearn.linear_model import LinearRegression
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from sklearn.metrics import mean_squared_error, r2_score


df = pd.read_csv("../data/ransom_payments.csv")

# print(df)

median_ransom = df['Median ransom payment'].to_numpy()
revenue = df['Annual Revenue (lower bound)'].to_numpy()


revenue = revenue.reshape(-1, 1)
# revenue = revenue[:, np.newaxis, 2]


# print(median_ransom)
# print(revenue)

reg = LinearRegression().fit(revenue, median_ransom)
print(reg.coef_)
print(reg.intercept_)

ransom_pred = reg.predict(revenue)

r2 = r2_score(median_ransom, ransom_pred)

print("r2 = ", r2)


samples = np.linspace(0,5*10**9, 1000)
# samples_outputs = samples * reg.coef_
# plt.plot(samples, samples_outputs)
# plt.show()

plt.plot(revenue, median_ransom)
plt.plot(samples, samples * reg.coef_ + 792145.1111738826)
plt.xlabel("revenue ($)")
plt.ylabel("ransom ($)")
plt.savefig('../figures/ransom_lr.png')