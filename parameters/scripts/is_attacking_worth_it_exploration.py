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

# for i in np.arange(0,1.1,0.01):
#     for j in np.arange(0,1.1,0.01):
        
mu=1.1356082157016467
sigma=1.1184432636889245

N = 100000
# defender_wealths = np.random.lognormal(mean=mu, sigma=sigma, size=N)

# print("defender_wealths", defender_wealths * 10 **9)

expected_posture_mu = 0.28
expected_posture_stddev = 0.10

# postures = np.random.normal(loc=expected_posture_mu, scale=expected_posture_stddev, size=N)

# P, W = np.meshgrid(postures, defender_wealths)

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

#################################################################################
#################################################################################
#################################################################################
# = 1.0
# aka it's virtually always worth it to attack
# except extreme edge cases e.g. wealth=0 which are irrelevant anyway
#################################################################################
#################################################################################
#################################################################################









# probably can ignore beneath here



# P = np.linspace(0,1,N)
# W = np.linspace(0,max(defender_wealths), N)

# # print(P)
# # print("-----")
# # print(W)


# # print("*********************")
# P, W = np.meshgrid(P, W)
# print(P)
# print("-----")
# print(W)

# # Z = []
# # for i in P:
# #     newarr = []
# #     for j in W:
# #         p_posture = norm.pdf(x = i, loc=expected_posture_mu,scale = expected_posture_stddev)
# #         p_wealth = lognorm.pdf(x=j,loc=mu, s=sigma, scale=np.exp(mu) ) * 10 ** 9
# #         newarr.append(p_posture * p_wealth)
# #     Z.append(newarr)

# p_postures = norm.pdf(x = P, loc=expected_posture_mu,scale = expected_posture_stddev)
# p_wealths = lognorm.pdf(x = W,loc=mu, s=sigma, scale=np.exp(mu) ) 
# # print("*********************")

# # print(p_postures)
# # print("-----")
# # print(p_wealths)

# Z = np.multiply(p_postures, p_wealths)

# np.set_printoptions(precision=2)
# print(Z)




# # print("-------------")

# # print(P)
# # print(W)

# # payoff = (ransom_base * W ** ransom_exp) * (1 - P)
# # cta = CTA_SCALING_FACTOR * P * W

# # Z = payoff * cta

# # fig, ax = plt.subplots(2)
# fig = plt.figure()
# ax = fig.add_subplot(projection='3d')
# ax.set_xlabel("postures")
# ax.set_ylabel("wealth")
# ax.set_zlabel("joint pdf")


# surf = ax.plot_surface(P, W, Z, cmap=cm.coolwarm,
#                        linewidth=0, antialiased=False, alpha = 0.5)


# payoff = (ransom_base * W ** ransom_exp) * (1 - P)
# cta = CTA_SCALING_FACTOR * P * W

# worth_it = (payoff > cta)
# print(worth_it)



# ax = fig.add_subplot(projection='3d')

# surf2 = ax.plot_surface(P, W, worth_it,
#                        linewidth=0, antialiased=False, alpha = 0.5)

# # plt.show()
# # plt.clf()

# # for i in np.arange(0,1.1,0.01):
# #     for j in np.arange(0,1.1,0.01):


