from sklearn.linear_model import LinearRegression
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib
from sklearn.metrics import mean_squared_error, r2_score

print('matplotlib: {}'.format(matplotlib.__version__))

df = pd.read_csv("../data/ransom_payments.csv")

median_ransom = df['Median ransom payment'].to_numpy()
revenue = df['Annual Revenue (lower bound)'].to_numpy()


revenue = revenue.reshape(-1, 1)

reg = LinearRegression().fit(revenue, median_ransom)
print(reg.coef_)
print(reg.intercept_)

ransom_pred = reg.predict(revenue)

r2 = r2_score(median_ransom, ransom_pred)

print("r2 = ", r2)


samples = np.linspace(0,5*10**9, 1000)


plt.plot(revenue, median_ransom)
plt.plot(samples, samples * reg.coef_ + reg.intercept_)
plt.xlabel("revenue ($)")
plt.ylabel("ransom ($)")
plt.title("ransom = {0} * revenue + {1}, \n r2 = {2} ".format(reg.coef_[0], reg.intercept_, r2))
plt.savefig('../figures/ransom_fitting/ransom_lr.png')