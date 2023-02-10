#!/usr/bin/python3

# Import libraries
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib.ticker import LinearLocator, FormatStrFormatter
import numpy as np
import sys

 
if len(sys.argv) != 2:
    print("Error: Wrong number of args")
    sys.exit(-1)

# CSVdata = open("averages_MANDATE=0.4.csv")
data = np.genfromtxt(sys.argv[1], delimiter=',')


fig = plt.figure()
ax = fig.gca(projection='3d')

# Make data.
X = np.linspace(0, 1, 11)
Y = np.linspace(0, 1, 11)
X, Y = np.meshgrid(X, Y)
Z = data


# Plot the surface.
# surf = ax.plot_surface(X, Y, Z, cmap=cm.coolwarm, linewidth=4, antialiased=False)
# surf = ax.plot_wireframe(X, Y, Z)
values = np.linspace(0.2, 1., X.ravel().shape[0])
colors = cm.rainbow(values)

surf = ax.bar3d(X.ravel(), Y.ravel(), Z.ravel()*0, 0.05, 0.05, Z.ravel(), color=colors, shade=True)

# Customize the z axis.
# ax.set_zlim(0.4, 0.1)
ax.set_xlabel('PREMIUM')
ax.set_ylabel('TAX')
ax.set_zlabel('Efficiency')
ax.zaxis.set_major_locator(LinearLocator(10))
ax.zaxis.set_major_formatter(FormatStrFormatter('%.02f'))

# Add a color bar which maps values to colors.
# fig.colorbar(surf, shrink=0.5, aspect=5)

plt.savefig("figures/" + sys.argv[1] + ".png")

# # Creating dataset
# x = np.linspace(0.0, 1.0, 11)
# y = np.linspace(0.0, 1.0, 11)
# z = (np.sin(x **2) + np.cos(y **2) )
# print(z)
# z = data 

# # Creating figure
# fig = plt.figure(figsize =(14, 9))
# ax = plt.axes(projection ='3d')
 
# # Creating plot
# plt.clf()
ax.clear()
surf = ax.plot_surface(X, Y, Z,  cmap=cm.coolwarm, linewidth=4, antialiased=True)
# Add a color bar which maps values to colors.

# ax.set_zlim(0, 1)
ax.set_xlabel('PREMIUM')
ax.set_ylabel('TAX')
ax.set_zlabel('Efficiency')
fig.colorbar(surf, shrink=0.5, aspect=5)

plt.savefig("figures/surface_" + sys.argv[1] + ".png")
 
# # show plot
# plt.show()