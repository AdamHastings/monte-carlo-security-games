import matplotlib.pyplot as plt
import matplotx
import numpy as np
import pandas as pd


opacity=0.6

b = '#636EFA'
y = '#FECB52'
r = '#EF553B' 

def plot_canary_vars(df):
    # # create data
    # rng = np.random.default_rng(0)
    # offsets = [1.0, 1.50, 1.60]
    # labels = ["no balancing", "CRV-27", "CRV-27*"]
    # x0 = np.linspace(0.0, 3.0, 100)
    # y = [offset * x0 / (x0 + 1) + 0.1 * rng.random(len(x0)) for offset in offsets]

    # # plot
    # with plt.style.context(matplotx.styles.dufte):
    #     for yy, label in zip(y, labels):
    #         plt.plot(x0, yy, label=label)
    #     plt.xlabel("distance [m]")
    #     matplotx.ylabel_top("voltage [V]")  # move ylabel to the top, rotate
    #     matplotx.line_labels()  # line labels to the right
    #     plt.tight_layout()
    #     plt.savefig('figures/canary_vars.png')
    #     plt.savefig('figures/canary_vars.pdf')

    cumulative_outputs = [
        ('d_cumulative_assets', 'defender assets', b, '-'),
        ('a_cumulative_assets', 'attacker assets', r, '-.'),
        ('i_cumulative_assets', 'insurer assets',  y, '-'),
        ('num_alive_defenders', 'num defenders',   b, '-.'),
        ('num_alive_attackers', 'num attackers',   r, '-'),
        ('num_alive_insurers',  'num insurers',    y, '-.')
    ]

    def normalize(arr):
        max_val = max(arr)
        return [x /max_val for x in arr]
        # return [x / arr[0] for x in arr]


    for c in list(zip(*cumulative_outputs))[0]:
        df[c] = df[c].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=int, sep=','))
        df[c] = df[c].apply(normalize)

    

    # plot
    with plt.style.context(matplotx.styles.dufte):

        for key, label, color, linestyle in cumulative_outputs:

            # consider shorest run instead?
            length = df[key].map(lambda x : len(x)).min()
            # length = df[key].map(lambda x : len(x)).max()

            x0 = np.arange(length)


            means = np.empty([length])
            for i in range(length):
                col = np.array([x[i] for x in df[key] if i < len(x)])
                # means[i] = np.percentile(col, 50) # technically this is the median now, not the mean...
                means[i] = col.mean()

            # print(label, means)


            plt.plot(x0, means, label=label, color=color, linestyle=linestyle)

        plt.xlabel("timestep")
        matplotx.ylabel_top("percentage")  # move ylabel to the top, rotate
        matplotx.line_labels()  # line labels to the right
        plt.xlim(0, 300)
        plt.ylim(-0.1, 1.1)
        plt.tight_layout()
        plt.savefig('figures/canary_vars.png')
        plt.savefig('figures/canary_vars.pdf')

    
    
    


    

if __name__=="__main__":
    df = pd.read_csv("../logs/fullsize_short.csv", header=0)
    plot_canary_vars(df)