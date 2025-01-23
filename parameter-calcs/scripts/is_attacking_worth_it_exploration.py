import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import lognorm
from scipy.stats import norm
from mpl_toolkits.mplot3d import Axes3D  # noqa: F401 unused import

from matplotlib import cm
from matplotlib.ticker import LinearLocator, FormatStrFormatter


ransom_base = 25365
ransom_exp = 0.254

CTA_SCALING_FACTOR = 0.0103

        
mu=1.1356082157016467
sigma=1.1184432636889245

N = 100000
defender_wealths = np.random.lognormal(mean=mu, sigma=sigma, size=N)
# np.set_printoptions(formatter={'float': '{:0.2e}'.format})

print("typical defender wealth = {:.2E}".format(defender_wealths[0] * 10 **6)) # in terms of thousands  ")
print("max(defender_wealths) = {:.2E}".format(max(defender_wealths) * 10 **6)) # in terms of thousands 

expected_posture_mu = 0.28
expected_posture_stddev = 0.10


worth_it_count = 0

for i in range(N):
    wealth = np.random.lognormal(mean=mu, sigma=sigma, size=1) * 10**9
    posture = np.random.normal(loc=expected_posture_mu, scale=expected_posture_stddev, size=1)
    payoff = (ransom_base * wealth ** ransom_exp) * (1 - posture)
    cta = CTA_SCALING_FACTOR * posture * wealth
    if (payoff > cta):
        worth_it_count += 1

print("worth_it_count: ", worth_it_count)
print("p(worth attacking)=", worth_it_count / N)