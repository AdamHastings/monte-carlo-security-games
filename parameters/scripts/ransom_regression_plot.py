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
    fig, ax = plt.subplots()

    ax.scatter(revenues[1:], ransom_payments, label="median ransom payment", color='red', marker="o")

    x = np.arange(0,5e9, 10000)
    rans_reg = [1.21e-3 * a + 792145 for a in x]
    ax.plot(x, rans_reg, color='red', alpha = 0.5, label=("${{{}}}x + {{{}}}$ ($R^2 = 0.928$)").format(1.21e-3, 792145) )

    ax.scatter(revenues, recovery_costs, label="median recovery cost", color='blue', marker="s")

    rec_reg = [230123 * pow(a, .125) for a in x]
    ax.plot(x, rec_reg, color='blue', alpha = 0.5, label=("${{{}}}x^{{{}}}$ ($R^2 = 0.865$)").format(230123, 0.125), linestyle='--' )

    ax.legend()
    plt.xlabel("annual revenue (billions)")
    plt.ylabel("cost (millions)")
    plt.xlim(-1e8,5.5e9)
    plt.ylim(-5e5, 7e6)
    
    plt.tight_layout()
    plt.savefig("../figures/ransom_regression/ransom_regression.png")
    plt.savefig("../figures/ransom_regression/ransom_regression.pdf")