import matplotlib.pyplot as plt
import numpy as np
from scipy import stats

from scipy.stats import lognorm

mu=1.1356082157016467
sigma=-1.1184432636889245

rands = np.random.rand(10000)
wealths = [stats.lognorm(mu, scale=np.exp(sigma)).ppf(x) for x in rands]
wealths = np.array(wealths)

ransoms = 25365 * wealths ** 0.254

plt.hist(ransoms, bins=100)
plt.xlabel("earnings")
plt.ylabel("count")
plt.savefig("../figures/simulated_attacker_earnings/simulated_attacker_earnings.png")