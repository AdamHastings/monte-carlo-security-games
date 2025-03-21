import matplotlib.pyplot as plt 
import matplotx
import numpy as np


revenues = [
    0.00,
    10000000.00,
    50000000.00,
    250000000.00,
    500000000.00,
    1000000000.00,
    5000000000.00,
]

recovery_costs = [
    1222487.00,
    1530886.00,
    2435089.00,
    2844296.00,
    2581460.00,
    2815793.00,
    3767731.00,
]

ransom_payments = [
    330000,
    220000,
    840000,
    2000000,
    3000000,
    6600000
]

with plt.style.context(matplotx.styles.dufte):

    # Override gridline and label colors to black
    plt.rcParams.update({
        'grid.color': 'black',  # Set gridlines to black
        'xtick.color': 'black',  # Set x-axis tick labels to black
        'ytick.color': 'black',  # Set y-axis tick labels to black
        'axes.labelcolor': 'black',  # Set axis labels to black
        'axes.edgecolor': 'black',  # Set the axis edge color to black
        'text.color': 'black'  # Default text color
    })

    fig, ax = plt.subplots()

    fig.set_size_inches(6,4.5)
    

    ax.scatter(revenues[1:], ransom_payments, label="median ransom payment", color='red', marker="o")

    x = np.arange(0,5e9, 10000)
    rans_reg = [1.21e-3 * a + 792145 for a in x]
    ax.plot(x, rans_reg, color='red', alpha = 0.5, label=(" ${{{}}}x + {{{}}}$\n ($R^2 = 0.928$)").format(1.21e-3, 792145) )

    ax.scatter(revenues, recovery_costs, label="median recovery cost", color='blue', marker="s")

    rec_reg = [230123 * pow(a, .125) for a in x]
    ax.plot(x, rec_reg, color='blue', alpha = 0.5, label=(" ${{{}}}x^{{{}}}$\n ($R^2 = 0.865$)").format(230123, 0.125), linestyle='--' )

    xticks = np.arange(0,6,1) * 1e9
    xlabels = ["$" + str(round(a / 1e9)) + "B" for a in xticks]
    ax.set_xticks(xticks, xlabels)

    yticks = np.arange(0,7,1) * 1e6
    ylabels = ["$" + str(round(a / 1e6)) + "M" for a in yticks]
    ax.set_yticks(yticks, ylabels)

    # ax.legend(framealpha=1.0)
    matplotx.line_labels(fontsize=12)  # line labels to the right

    scatter_legend_handles = [plt.Line2D([0], [0], marker='o', color='red', label='median ransom payment', linestyle='None'), plt.Line2D([0], [0], marker='s',  color='blue', label='median recovery cost', linestyle='None')]
    ax.legend(handles=scatter_legend_handles,  loc='center left', framealpha=1.0, bbox_to_anchor=(.4, 0.15))

    plt.xlabel("annual revenue")
    plt.ylabel("cost per incident")
    plt.xlim(-1e8,5.2e9)
    plt.ylim(-5e2, 7e6)
    plt.gca().xaxis.grid(True)
    plt.tight_layout()
    plt.savefig("../figures/ransom_regression/ransom_regression.png")
    plt.savefig("../figures/ransom_regression/ransom_regression.pdf")