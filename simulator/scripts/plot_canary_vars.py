import matplotlib.pyplot as plt
import matplotx
import numpy as np
import pandas as pd
import sys
import copy
import os
import os.path


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
        ('a_cumulative_assets', 'attackers $', r, '-'),
        ('i_cumulative_assets', 'insurers $',  y, '-'),
        ('num_alive_defenders', '# defenders',   b, '-.'),
        ('num_alive_attackers', '# attackers',   r, '-.'),
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
    # with plt.style.context(matplotx.styles.dufte):
    plt.figure(figsize=(6,4))

    for key, label, color, linestyle in cumulative_outputs:

        # consider shorest run instead?
        length = int(df[key].map(lambda x : len(x)).median())
        # length = df[key].map(lambda x : len(x)).max()



        means = np.empty([length])
        fifthpct = np.empty([length])
        ninetyfifthpct = np.empty([length])
        for i in range(length):
            col = np.array([x[i] for x in df[key] if i < len(x)])
            
            means[i] = np.percentile(col, 50) # technically this is the median now, not the mean...
            fifthpct[i] = np.percentile(col, 5)
            ninetyfifthpct[i] = np.percentile(col, 95)

        x = np.arange(length)

        plt.fill_between(x, fifthpct, ninetyfifthpct, color=color, alpha=0.5, edgecolor='none')
        plt.plot(x, means, label=label, color=color, linestyle=linestyle)

    plt.xlabel("timestep")
    matplotx.ylabel_top("percentage")  # move ylabel to the top, rotate
    # matplotx.line_labels()  # line labels to the right
    plt.legend()
    # plt.xlim(0, 300)
    # plt.ylim(0, 1.0)
    plt.gca().xaxis.grid(True)
    plt.tight_layout()

    basetitle = 'canary_vars'
    dirname = 'figures'
    subdirname = df['folder'][0]
    path = dirname + '/' + subdirname 
    
    if not os.path.isdir(path):
        os.mkdir(path)

    plt.savefig(path + '/' + basetitle + '.png')
    plt.savefig(path + '/' + basetitle + '.pdf')


def plot_p_attacks(df):


    df['p_pairing']                       = df['p_pairing'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=float, sep=','))
    df['p_attacked']                      = df['p_attacked'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=float, sep=','))
    df['p_looted']                        = df['p_looted'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=float, sep=','))
    # df['insurer_estimate_p_pairing']      = df['insurer_estimate_p_pairing'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=float, sep=','))
    # df['estimated_probability_of_attack'] = df['estimated_probability_of_attack'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=float, sep=','))
    df['cumulative_defender_avg_posture'] = df['cumulative_defender_avg_posture'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=float, sep=','))


    # plot p_parings/p_attacks
    plt.clf()

    attack_ps = [
        ('p_pairing', '% paired', g, '-'),
        ('p_attacked', '% attacked', r, '-'),
        ('p_looted', '% ransomed', k, '-'),
        # ('insurer_estimate_p_pairing', 'I\'s est. p_attack', y, '-'),
        # ('estimated_probability_of_attack', 'D\'s est. p_attack', b, '-'),
        ('cumulative_defender_avg_posture', 'average\nsecurity\nposture', '#0000FF', '-')
    ]

    with plt.style.context(matplotx.styles.dufte):
        plt.figure(figsize=(6,4))
        for key, label, color, linestyle in attack_ps:

            length = int(df[key].map(lambda x : len(x)).median())

            means = np.empty([length])
            fifthpct = np.empty([length])
            ninetyfifthpct = np.empty([length])
            for i in range(length):
                col = np.array([x[i] for x in df[key] if i < len(x)])
                means[i] = col.mean()
                fifthpct[i] = np.percentile(col, 5)
                ninetyfifthpct[i] = np.percentile(col, 95)

            # print(label, means)

            x = np.arange(length)
            # too confusing to look at 
            # plt.fill_between(x, fifthpct, ninetyfifthpct, color=color, alpha=0.5, edgecolor='none')
            plt.plot(x, means, label=label, color=color, linestyle=linestyle)

        plt.xlabel("timestep")
        matplotx.ylabel_top("")  # move ylabel to the top, rotate
        matplotx.line_labels()  # line labels to the right
        # plt.xlim(0, 300)
        plt.ylim(0, 1.0)
        # fig.set_size_inches(7,3.5)
        # plt.figure(figsize=(7,3))
        plt.gca().xaxis.grid(True)
        plt.tight_layout()
        # plt.show()

        basetitle = 'canary_vars_p_attack'
        dirname = 'figures'
        subdirname = df['folder'][0]
        path = dirname + '/' + subdirname 
        # plt.set_xticks(np.arange(0,5000,1000))
        # plt.xticks(np.arange(0,4000,1000))
        
        if not os.path.isdir(path):
            os.mkdir(path)

        # plt.show() # uncomment for zoom in

        plt.savefig(path + '/' + basetitle + '.png')
        plt.savefig(path + '/' + basetitle + '.pdf')
        


    

if __name__=="__main__":

    if (len(sys.argv) == 2):
        filename = sys.argv[1]
    else:
        print("Incorrect number of args! Example:")
        print("$ python3 run_all.py <path_to_log_file.csv>")
        sys.exit(1)

    df = pd.read_csv(filename, header=0)
    filename = filename.replace("../logs/", "")
    filename = filename.replace(".csv", "")
    df['folder'] = filename

    plot_p_attacks(copy.deepcopy(df))
    plot_canary_vars(copy.deepcopy(df))
