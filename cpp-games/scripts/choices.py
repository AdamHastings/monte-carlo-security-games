import matplotlib.pyplot as plt
import matplotx
import numpy as np
import pandas as pd


opacity=0.6

b = '#636EFA'
y = '#FECB52'
r = '#EF553B' 

def choices(df):
    plt.clf()

    # df = df['cumulative_round_policies_purchased','cumulative_round_defenses_purchased','cumulative_round_do_nothing']

    cumulative_policies_medians = []
    cumulative_defenses_medians = []
    cumulative_nothings_medians = []
    
    # for label, c, l in zip(['cumulative_round_policies_purchased','cumulative_round_defenses_purchased','cumulative_round_do_nothing'], ['b', 'r', 'y'], ['--', '-', '-.']):

    # frame = label

    # TODO remove deepcopy, do this in run_all?
    df['cumulative_round_policies_purchased'] = df['cumulative_round_policies_purchased'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=int, sep=','))
    df['cumulative_round_defenses_purchased'] = df['cumulative_round_defenses_purchased'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=int, sep=','))
    df['cumulative_round_do_nothing'] = df['cumulative_round_do_nothing'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=int, sep=','))




        

    # consider shorest run instead?
    length = df['cumulative_round_policies_purchased'].map(lambda x : len(x)).min()


    for i in range(length):
        col = [x[i] for x in df['cumulative_round_policies_purchased'] if i < len(x)]
        cumulative_policies_medians.append(np.percentile(col, 50))

        col = [x[i] for x in df['cumulative_round_defenses_purchased'] if i < len(x)]
        cumulative_defenses_medians.append(np.percentile(col, 50))

        col = [x[i] for x in df['cumulative_round_do_nothing'] if i < len(x)]
        cumulative_nothings_medians.append(np.percentile(col, 50))


    x = range(length)
    
    # with plt.style.context(matplotx.styles.dufte):
    plt.style.use("fivethirtyeight")    

        
    stacks = plt.stackplot(x,cumulative_nothings_medians,cumulative_policies_medians, cumulative_defenses_medians, labels=['neither','insurance','security'], colors=[r, y, b], edgecolor='#00000044', lw=.1)


    hatches=["xxx", "ooo", "+++"]
    for stack, hatch in zip(stacks, hatches):
        stack.set_hatch(hatch)

    plt.legend()
    plt.xlabel("timestep")
    plt.ylabel("count")
    plt.tight_layout()


    plt.savefig('figures/choices.png')
    plt.savefig('figures/choices.pdf')



if __name__=="__main__":
    df = pd.read_csv("../logs/fullsize_short.csv", header=0)
    choices(df)    