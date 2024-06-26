import matplotlib.pyplot as plt
import matplotx
import numpy as np
import pandas as pd
import sys
import copy


opacity=0.6

b = '#636EFA'
y = '#FECB52'
r = '#EF553B' 
g = '#00CC96'
k = '#000000'

def plot_canary_vars(df):

    plt.clf()

    cumulative_outputs = [
        ('d_cumulative_assets', 'defenders $', b, '-'),
        ('a_cumulative_assets', 'attackers $', r, '-.'),
        ('i_cumulative_assets', 'insurers $',  y, '-'),
        ('num_alive_defenders', '# defenders',   b, '-.'),
        ('num_alive_attackers', '# attackers',   r, '-'),
        ('num_alive_insurers',  '# insurers',    y, '-.')
    ]

    def normalize(arr):
        max_val = max(arr)
        return [x /max_val for x in arr]

    df['d_cumulative_assets'] = df['d_cumulative_assets'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=int, sep=','))
    df['a_cumulative_assets'] = df['a_cumulative_assets'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=int, sep=','))
    df['i_cumulative_assets'] = df['i_cumulative_assets'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=int, sep=','))
    df['num_alive_defenders'] = df['num_alive_defenders'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=int, sep=','))
    df['num_alive_attackers'] = df['num_alive_attackers'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=int, sep=','))
    df['num_alive_insurers'] = df['num_alive_insurers'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=int, sep=','))

    for c in list(zip(*cumulative_outputs))[0]:
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


def plot_p_attacks(df):


    df['p_pairing']                       = df['p_pairing'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=float, sep=','))
    df['p_attacked']                      = df['p_attacked'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=float, sep=','))
    df['p_looted']                        = df['p_looted'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=float, sep=','))
    df['insurer_estimate_p_pairing']      = df['insurer_estimate_p_pairing'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=float, sep=','))
    df['estimated_probability_of_attack'] = df['estimated_probability_of_attack'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=float, sep=','))
    

    # plot p_parings/p_attacks
    plt.clf()

    attack_ps = [
        ('insurer_estimate_p_pairing', 'I\'s est. p_attacked', y, '-'),
        ('p_pairing', 'p_pairing', g, '-'),
        ('p_attacked', 'p_attacked', r, '-'),
        ('p_looted', 'p_looted', k, '-'),
        ('estimated_probability_of_attack', 'D\'s est. p_looted', b, '-')
    ]

    with plt.style.context(matplotx.styles.dufte):

        for key, label, color, linestyle in attack_ps:

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
        matplotx.ylabel_top("")  # move ylabel to the top, rotate
        matplotx.line_labels()  # line labels to the right
        # plt.xlim(0, 300)
        # plt.ylim(-0.1, 1.1)
        plt.tight_layout()
        plt.savefig('figures/canary_vars_p_attack.png')
        plt.savefig('figures/canary_vars_p_attack.pdf')
    

if __name__=="__main__":

    if len(sys.argv) == 1:
        df = pd.read_csv("../logs/fullsize_short.csv", header=0)
    else:
        df = pd.read_csv(sys.argv[1], header=0)

    plot_p_attacks(copy.deepcopy(df))
    plot_canary_vars(copy.deepcopy(df))
