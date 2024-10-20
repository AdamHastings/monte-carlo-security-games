import numpy as np
import matplotlib.pyplot as plt
import math
from matplotlib import rc
import matplotx


with plt.style.context(matplotx.styles.dufte):
    x = np.arange(0, .1, 0.001)
    # x = np.append(x, 1)

    SCALING_FACTOR = 25
    plt.figure(figsize=(6,3))


    # plt.figure(figsize=(4,3))
    ax = plt.subplot(111)


    y = [math.erf(xi * SCALING_FACTOR) for xi in x]
    ax.plot(x, y,label="y=erf(25x)")

    # y2 = [(xi / (1 + abs(xi))) for xi in x]
    # plt.plot(x, y2)

    # plt.plot(x, [math.erf(xi) for xi in x])


    plt.xlabel("Total investment (capex + opex)")
    plt.ylabel("security posture")
    # plt.ylabel("security posture \n " + r'\textcolor{red}{in-model average posture} ' )                 ")
    # plt.ylabel(r'\textcolor{red}{Today} '+
    #            r'\textcolor{green}{is} '+
    #            r'\textcolor{blue}{cloudy.}')

    ax.axvline(x = 0.01, ymin = 0.0, ymax = 0.28, color = 'r')

    ax.axhline(y = 0.28, xmin =0, xmax = 0.10, color = 'r')
   


    # ticks = np.arange(0, .11, .05)
    # ticks = np.insert(ticks,1, 0.01)
    ticks = [0, 0.01, 0.05, 0.1]
    labels = ["{:.0%}".format(x) for x in ticks]
    labels[0] = ""
    id_tick_change_colour = 1
    labels[id_tick_change_colour] = "1% \nreal-world average\n security investment"

    ax.set_xticks(ticks, labels)
    plt.setp(ax.get_xticklabels()[id_tick_change_colour], color='red')


    # yticks = np.arange(0, 1.1, .5)
    # yticks = np.insert(yticks, 1, 0.28)
    yticks = np.array([0, 0.28, .5, 0.75, 1])
    id_tick_change_colour = 1
    ylabels = [str(x) for x in yticks]
    ylabels[0] = ""
    ylabels[id_tick_change_colour] = "\n\n0.28 \nin-model\naverage"

    ax.set_yticks(yticks, ylabels)
    plt.setp(ax.get_yticklabels()[id_tick_change_colour], color='red')


    matplotx.line_labels()  # line labels to the right
    plt.gca().xaxis.grid(True)
    plt.tight_layout()
    plt.savefig("../figures/erf/erf.png")
    plt.savefig("../figures/erf/erf.pdf")
