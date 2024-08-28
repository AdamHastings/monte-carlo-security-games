import matplotlib.pyplot as plt
import numpy as np
from scipy import stats
from scipy.stats import lognorm
from scipy.stats import norm


from scipy.stats import lognorm

np.set_printoptions(formatter={'float': '{:0.2e}'.format})


mu=1.1356082157016467
sigma=1.1184432636889245

inequality = 0.001
num_attackers = 500

expected_posture_mu = 0.28
expected_posture_stddev = 0.10

f = open("simulate_attackers_earnings.txt", "w")

attackers_wealths = np.random.lognormal(mean=mu, sigma=sigma, size=num_attackers) * 10 ** 9 * inequality
# f.write("attacker_wealth: ", "{:e}".format(attackers_wealth))

num_attacks = 100 # TODO TODO TODO this is assuming that the attacker had enough to gamble for each of these attacks?
prob_attack_success = 1 - expected_posture_mu
num_success_attacks = (int) (num_attacks * prob_attack_success)

wealths = np.random.lognormal(mean=mu, sigma=sigma, size=num_attacks)
wealths = wealths * 10**9

postures = np.random.normal(loc=expected_posture_mu, scale=expected_posture_stddev, size=num_attacks)

target_rc_ratio = 0.333

MAGIC_SCALAR = target_rc_ratio * (1 - expected_posture_mu) / expected_posture_mu




ransom_base = 25365
ransom_exp = 0.254

ransoms = ransom_base * wealths ** ransom_exp

costs_to_attack = [MAGIC_SCALAR * x[0] * x[1] for x in zip(ransoms, postures)]
# f.write(costs_to_attack)
total_costs_to_attack = np.sum(costs_to_attack)
total_revenue = np.sum(ransoms[0: num_success_attacks])

f.write("sample revenue: {:e}\n".format(total_revenue))
f.write("sample costs: {:e}\n".format(total_costs_to_attack))


f.write("sample C/R = {}\n".format(total_costs_to_attack/total_revenue))
f.write("")






# expected_assets = np.exp(mu + (sigma **2 / 2) ) * 10**9

# f.write("expected_assets: {:e}\n".format(expected_assets))

# analytic_revenue = num_success_attacks * ransom_base * expected_assets ** ransom_exp


# analytic_magic_scalar = analytic_revenue / (target_rc_ratio * num_attacks * expected_posture_mu * expected_assets )

# analytic_costs = num_attacks * analytic_magic_scalar * expected_posture_mu * expected_assets


# analytic_rc_ratio = analytic_revenue / analytic_costs



# f.write("analytic_revenue: {:e}\n".format(analytic_revenue))
# f.write("analytic_costs:{:e}\n".format(analytic_costs))
# f.write("analytic_rc_ratio: {}\n".format(analytic_rc_ratio))
f.write("MAGIC_SCALAR: {}\n".format(MAGIC_SCALAR))

f.write("Pr[gain > cost] = Pr[(1-Posture) > MAGIC_SCALAR * Posture]  = {}\n".format( norm.cdf(1/(1 + MAGIC_SCALAR), loc=expected_posture_mu, scale=expected_posture_stddev )))

f.write("\n")
f.write("Example cost_to_attacks:\n")
for i in range(100):
    expected_loot = ransoms[i] * (1 - expected_posture_mu) 
    expected_cost_to_attack = MAGIC_SCALAR * ransoms[i] * postures[i]
    worth_attacking = "YES" if expected_loot > expected_cost_to_attack else "NO"
    have_enough = "YES" if expected_cost_to_attack < attackers_wealths[i] else "NO"
    fstring = "  -- wealth = {:.2e}\t posture = {}\t\t expected cost to attack = {:.2e}\t ransom = {:.2e}\t expected_loot = {:2e}\t worth attacking? {}\t have_enough? {}\n".format(wealths[i], round(postures[i],2), expected_cost_to_attack, ransoms[i], expected_loot, worth_attacking, have_enough)
    f.write(fstring)


plt.hist(ransoms, bins=100)
plt.xlabel("earnings")
plt.ylabel("count")
plt.savefig("../figures/simulated_attacker_earnings/simulated_attacker_earnings.png")

f.close()