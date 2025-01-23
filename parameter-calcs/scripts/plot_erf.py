import numpy as np
import matplotlib.pyplot as plt
import math
from matplotlib import rc
import matplotx


with plt.style.context(matplotx.styles.dufte):

    # Override gridline and label colors to black
    plt.rcParams.update({
        'grid.color': 'black',  # Set gridlines to black
        'xtick.color': 'black',  # Set x-axis tick labels to black
        'ytick.color': 'black',  # Set y-axis tick labels to black
        'axes.labelcolor': 'black',  # Set axis labels to black
        'axes.edgecolor': 'black'  # Set the axis edge color to black
    })
    
    x = np.arange(0, .1, 0.001)

    SCALING_FACTOR = 25
    plt.figure(figsize=(6,3))


    ax = plt.subplot(111)


    y = [math.erf(xi * SCALING_FACTOR) for xi in x]
    ax.plot(x, y,label="y=erf(25x)")

    plt.xlabel("Total investment (capex + opex)")
    plt.ylabel("security posture")

    ax.axvline(x = 0.01, ymin = 0.0, ymax = 0.28, color = 'r')
    ax.axhline(y = 0.28, xmin =0, xmax = 0.10, color = 'r')

    ticks = [0, 0.01, 0.05, 0.1]
    labels = ["{:.0%}".format(x) for x in ticks]
    labels[0] = ""
    id_tick_change_colour = 1
    labels[id_tick_change_colour] = "1% \nreal-world average\n security investment"

    ax.set_xticks(ticks, labels)
    plt.setp(ax.get_xticklabels()[id_tick_change_colour], color='red')

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
