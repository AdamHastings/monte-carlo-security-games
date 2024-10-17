import matplotlib.pyplot as plt
import matplotx
import numpy as np
import pandas as pd
import sys
import os
import os.path

opacity=0.6

b = '#636EFA'
y = '#FECB52'
r = '#EF553B' 

def choices(df):

    plt.figure(figsize=(6,2.75))
    # TODO remove deepcopy, do this in run_all?
    df['cumulative_round_policies_purchased'] = df['cumulative_round_policies_purchased'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=int, sep=','))
    df['cumulative_round_defenses_purchased'] = df['cumulative_round_defenses_purchased'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=int, sep=','))
    df['cumulative_round_do_nothing'] = df['cumulative_round_do_nothing'].apply(lambda x: np.fromstring(x.replace('[','').replace(']',''), dtype=int, sep=','))


    plt.clf()
    # plt.figure(figsize=(4,3))

    # df = df['cumulative_round_policies_purchased','cumulative_round_defenses_purchased','cumulative_round_do_nothing']

    cumulative_policies_medians = []
    cumulative_defenses_medians = []
    cumulative_nothings_medians = []
    
    # for label, c, l in zip(['cumulative_round_policies_purchased','cumulative_round_defenses_purchased','cumulative_round_do_nothing'], ['b', 'r', 'y'], ['--', '-', '-.']):

    # frame = label
        

    # consider shorest run instead?
    length = int(df['cumulative_round_policies_purchased'].map(lambda x : len(x)).median())


    for i in range(length):
        col = [x[i] for x in df['cumulative_round_policies_purchased'] if i < len(x)]
        cumulative_policies_medians.append(np.percentile(col, 50))

        col = [x[i] for x in df['cumulative_round_defenses_purchased'] if i < len(x)]
        cumulative_defenses_medians.append(np.percentile(col, 50))

        col = [x[i] for x in df['cumulative_round_do_nothing'] if i < len(x)]
        cumulative_nothings_medians.append(np.percentile(col, 50))


    x = range(length)
    
    # fig, ax = plt.subplots(facecolor=(0,0,0))
    # ax.set_facecolor('#ffffff')

    # with plt.style.context(matplotx.styles.dufte):
    # plt.style.use("bmh")
    # fig.patch.set_facecolor('white')

    with plt.style.context(matplotx.styles.dufte):


        if "mandatory_insurance" in df['folder'][0]:
            print(" --- skipping insurance choices (mandatory)")
            # cumulative_policies_medians = [0 for _ in range(length)]
            stacks = plt.stackplot(x,cumulative_nothings_medians, cumulative_defenses_medians, labels=['neither','security'], colors=[r, b], edgecolor='#00000044', lw=.1)
        else:  
            stacks = plt.stackplot(x,cumulative_nothings_medians,cumulative_policies_medians, cumulative_defenses_medians, labels=['neither','insurance','security'], colors=[r, y, b], edgecolor='#00000044', lw=.1)


        # hatches=["", "---", "..."]
        hatches=["","",""]
        # hatches = ["", "+++", "..."]
        for stack, hatch in zip(stacks, hatches):
            stack.set_hatch(hatch)

        plt.legend(framealpha=1.0)
        plt.xlabel("timestep")
        plt.ylabel("count")
        plt.gca().xaxis.grid(True)
        plt.tight_layout()

        basetitle = 'choices'
        dirname = 'figures'
        subdirname = df['folder'][0]
        path = dirname + '/' + subdirname 
        
        if not os.path.isdir(path):
            os.mkdir(path)

        plt.savefig(path + '/' + basetitle + '.png')
        plt.savefig(path + '/' + basetitle + '.pdf')

        plt.clf()
        # plt.figure(figsize=(4,3))

        cumulative_policies_medians_pcts = []
        cumulative_defenses_medians_pcts = []
        cumulative_nothings_medians_pcts = []
        
        for i in range(length):
            col = [x[i] for x in df['cumulative_round_policies_purchased'] if i < len(x)]
            insurance = np.percentile(col, 50)

            if "mandatory_insurance" in df['folder'][0]:
                insurance = 0

            col = [x[i] for x in df['cumulative_round_defenses_purchased'] if i < len(x)]
            defense = np.percentile(col, 50)

            col = [x[i] for x in df['cumulative_round_do_nothing'] if i < len(x)]
            neither = np.percentile(col, 50)


            tsum = insurance + defense + neither
            cumulative_policies_medians_pcts.append( insurance / tsum)
            cumulative_defenses_medians_pcts.append( defense / tsum)
            cumulative_nothings_medians_pcts.append( neither / tsum)

        if "mandatory_insurance" in df['folder'][0]:
            print(" --- skipping insurance choices (mandatory)")
            stacks = plt.stackplot(x,cumulative_nothings_medians_pcts, cumulative_defenses_medians_pcts, labels=['neither','security'], colors=[r, b], edgecolor='#00000044', lw=.1)
        else:
            stacks = plt.stackplot(x,cumulative_nothings_medians_pcts,cumulative_policies_medians_pcts, cumulative_defenses_medians_pcts, labels=['neither','insurance','security'], colors=[r, y, b], edgecolor='#00000044', lw=.1)

        # hatches=["", "---", "..."]
        # hatches = ["", "+++", "..."]
        hatches = ["","",""]
        for stack, hatch in zip(stacks, hatches):
            stack.set_hatch(hatch)

        plt.legend(framealpha=1.0)
        plt.xlabel("timestep")
        plt.ylabel("percentage")
        plt.tight_layout()
        plt.gca().xaxis.grid(True)
        plt.savefig(path + '/' + basetitle + '_pcts.png')
        plt.savefig(path + '/' + basetitle + '_pcts.pdf')


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

    # print(df['folder'])
    # print(df['folder'][0])

    choices(df)    