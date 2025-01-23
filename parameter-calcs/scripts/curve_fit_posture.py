import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import scipy as sp 
from scipy import stats 
from scipy.stats import norm
from scipy.optimize import curve_fit 


df = pd.read_csv("../data/posture_by_industry.csv")


postures = []

for index, row in df.iterrows():
    count = row['N']
    attack_failure = row['The attack was stopped before data was encrypted']
    vals = [attack_failure] * count
    postures.extend(vals)


plt.hist(postures, bins=len(df), density=True, alpha=0.6, color='g',  ec='black')



mu_, std_ = norm.fit(postures)


xmin, xmax = plt.xlim()
x = np.linspace(xmin, xmax, 100)
p = norm.pdf(x, mu_, std_)
plt.plot(x, p, 'k', linewidth=2)

title = "Fit results: mu = %.2f,  std = %.2f" % (mu_, std_)
plt.title(title)
plt.xlabel("Attack failures")
plt.ylabel("PDF")

plt.savefig("../figures/posture_fitting/hist.png")