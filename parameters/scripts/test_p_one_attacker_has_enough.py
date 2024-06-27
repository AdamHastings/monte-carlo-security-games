import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import binom
from math import *

mu= 100
sigma=10

S = 100

samples = np.random.normal(loc=mu, scale = sigma, size = S)
print(samples)
print("max: ", max(samples))

x = 120

p_s_greater_than_x = (1.0 + erf((x - mu)/ (sigma * sqrt(2.0)))) / 2.0
print("p_s_greater_than_x: ", p_s_greater_than_x)


p_S_failures_in_a_row = p_s_greater_than_x ** S
print("p_S_failures_in_a_row:", p_S_failures_in_a_row)

k = 1 # number of needed successes
n = 100 # number of trials 


p_any_s_greater_than_x = binom.cdf(x, n, p_s_greater_than_x)
