import matplotlib.pyplot as plt
import numpy as np
from scipy import stats
from scipy.stats import lognorm


from scipy.stats import lognorm

mu=1.1356082157016467
sigma=1.1184432636889245

inequality = 0.001
num_attackers = 50

# attackers_wealth = np.max(np.random.lognormal(mean=mu, sigma=sigma, size=num_attackers)) * 10 ** 9 * inequality
# print("attacker_wealth: ", "{:e}".format(attackers_wealth))


num_attacks = 100 # TODO TODO TODO this is assuming that the attacker had enough to gamble for each of these attacks?
prob_attack_success = 1 - 0.28
num_success_attacks = (int) (num_attacks * prob_attack_success)

wealths = np.random.lognormal(mean=mu, sigma=sigma, size=num_attacks)
wealths = wealths * 10**9

postures = np.random.normal(loc=0.28, scale=0.10, size=num_attacks)


MAGIC_SCALAR = 0.012


costs_to_attack = [MAGIC_SCALAR * x[0] * x[1] for x in zip(wealths, postures)]
print(costs_to_attack)
total_costs_to_attack = np.sum(costs_to_attack)


ransom_base = 25365
ransom_exp = 0.254

successful_attacks = wealths[0: num_success_attacks]
ransoms = ransom_base * wealths ** ransom_exp

total_revenue = np.sum(ransoms)
print("total_revenue: ", "{:e}".format(total_revenue))

print("R/C = ", total_revenue/total_costs_to_attack)


plt.hist(ransoms, bins=100)
plt.xlabel("earnings")
plt.ylabel("count")
plt.savefig("../figures/simulated_attacker_earnings/simulated_attacker_earnings.png")