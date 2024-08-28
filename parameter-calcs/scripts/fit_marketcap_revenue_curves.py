#!/bin/python3
# import numpy as np
# from scipy.optimize import curve_fit
import numpy as np
import random
from scipy import stats
from scipy.stats import poisson
from scipy.optimize import minimize
import pandas as pd
import powerlaw
import matplotlib.pyplot as plt
from scipy.stats import lognorm
from scipy.optimize import curve_fit
from scipy.stats import kstest
from scipy.stats import probplot


# data obtained from https://companiesmarketcap.com/largest-companies-by-revenue
df = pd.read_csv('../data/global_marketcap_revenue_earnings.csv')


# Example data
data = df['revenue'].to_numpy()


# in terms of millions, for the sake of not overflowing computationally when generating 
# shouldn't this be Billions, not millions?
data = data / 10**9

# # Define the negative log likelihood function for Poisson distribution
# def neg_log_likelihood(params, data):
#     # Extract the parameter lambda from the params
#     lam = params[0]
#     # Calculate the negative log likelihood
#     neg_ll = -np.sum(poisson.logpmf(data, lam))
#     return neg_ll

# # Initial guess for the parameter lambda
# initial_guess = [2.0]

# # Fit the Poisson distribution using maximum likelihood estimation
# result = minimize(neg_log_likelihood, initial_guess, args=(data,))
# lambda_hat = result.x[0]

# print("Estimated lambda:", lambda_hat)


# # Fit the power law distribution
# fit = powerlaw.Fit(data)

# # Print the estimated parameters
# print("Alpha:", fit.alpha)
# print("xmin:", fit.xmin)


# Fit the lognormal distribution
params = lognorm.fit(data)

plt.plot(data)

# Extract the parameters
mu, sigma = params[0], params[1]

printstr = "mu=" + str(mu) + ", sigma=" + str(sigma)
print(printstr)
f = open("WEALTH_params.txt", 'w')
f.write(printstr)
f.close()


# create some example values with the found mu and sigma
rands = np.random.rand(100)
samples = [stats.lognorm(mu, scale=np.exp(sigma)).ppf(x) for x in rands]
print("\nsamples = \n")
samples.sort()
print(samples)
print("")


# Plot the histogram of the data
plt.hist(data, bins=50, density=True, alpha=0.6, color='g')

# Generate points for the fitted lognormal distribution
xmin, xmax = plt.xlim()
x = np.linspace(xmin, xmax, 100)
pdf = lognorm.pdf(x, mu, sigma)

# Plot the fitted lognormal distribution
plt.plot(x, pdf, 'k', linewidth=2)

plt.xlabel('Value')
plt.ylabel('Density')
plt.title('Histogram of Lognormal Distribution')
plt.savefig("../figures/wealthfitting/lognormal.png")
plt.savefig("../figures/wealthfitting/lognormal.pdf")

# Perform Kolmogorov-Smirnov test
# what are considered good scores here?
ks_statistic, p_value = kstest(data, 'lognorm', params)

print("Kolmogorov-Smirnov test:")
print("KS statistic:", ks_statistic)
print("P-value:", p_value)

plt.clf()

fig = plt.figure(figsize=(6, 6))
ax = fig.add_subplot(111)
probplot(data, dist=lognorm, sparams=(mu, sigma), plot=ax)
ax.get_lines()[1].set_linestyle('--')  # Change the linestyle of the reference line
plt.xlabel('Theoretical Quantiles')
plt.ylabel('Ordered Values')
plt.title('Q-Q Plot of Lognormal Distribution')
plt.grid(True)
plt.savefig('../figures/wealthfitting/lognormal-qq.png')
plt.savefig('../figures/wealthfitting/lognormal-qq.pdf')

plt.clf()

# # Fit the power law distribution
# # Power law is no good---not considering any more.
# fit = powerlaw.Fit(data)

# # Generate synthetic data from the fitted distribution
# synthetic_data = fit.power_law.generate_random(len(data))

# # Create a Q-Q plot comparing the observed data to the synthetic data
# fig = plt.figure(figsize=(6, 6))
# ax = fig.add_subplot(111)
# probplot(data, dist='powerlaw', sparams=(fit.alpha,), plot=ax)
# plt.xlabel('Theoretical Quantiles')
# plt.ylabel('Ordered Values')
# plt.title('Q-Q Plot of Power Law Distribution')
# plt.grid(True)
# plt.savefig('../figures/wealthfitting/powerlaw-qq.png')
# plt.savefig('../figures/wealthfitting/powerlaw-qq.pdf')