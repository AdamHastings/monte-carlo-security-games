#!/usr/bin/python3
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as mtick


def make_cdfs(bigdf):
    # For each PREMIUM:
    for p in  ["0.0", "0.1", "0.2", "0.3", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9", "1.0"]:


        # each of these will be a unique plot
        plt.figure(1)
        plt.clf()

        fig2 = plt.figure(2)
        plt.clf()


        # For each MANDATE
        for m in ["0.0", "0.1", "0.2", "0.3", "0.4", "0.5"]:

            # For t in TAX
            # Only considering TAX=0.0 for now
            for t in ["0.0"]: # ["0.0", "0.1", "0.2", "0.3", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9", "1.0"]:   


                # there's the problem --- old figs were NOT looking at "trimmed" games!
                # ofname = '../trimmed/MANDATE=' + m + "_TAX=" + t + "_PREMIUM=" + p + ".csv"
                # print(ofname)    

                # df = pd.read_csv(ofname)
                df = bigdf.loc[(bigdf['MANDATE'] == float(m)) & (bigdf['PREMIUM'] == float(p)) & (bigdf['TAX'] == float(t))]
                print(df.info())

                # initial * (1 - MANDATE) = d_init
                # initial = d_init / (1 - MANDATE)
                # the amount Defenders have *before* any mandate is applied
                df['d_init_init'] = df['d_init'] / (1 - df['MANDATE'])
                df['loss'] = df['d_end'] / df['d_init_init']
                df['efficiency'] = 1 - df['loss']


                plt.figure(1)
                hist, bins = np.histogram(df['loss'], bins = 100, density = True )
                # print(bins)
                dx = bins[1] - bins[0]
                cdf = np.cumsum(hist)*dx

                # print(cdf)
                plt.plot(cdf, label="MANDATE=" + str(int(float(m)*100)) + "%")

                plt.figure(2)
                

                d_inits = df['d_init'].to_numpy() / (1 - float(m))
                # print(d_inits)
                d_ends = df['d_end'].to_numpy()

                ratios = []
                for start,finish in zip(d_inits, d_ends):
                    ratios.append(((start - finish)/start) * 100)
                
                N = len(ratios)
                # print("N = " + str(N))
                X2 = np.sort(ratios)
                F2 = np.array(range(N))/float(N) * 100
                # print(X2[0], X2[-1])
                
                # F2 = np.delete(F2,np.arange(100))
                count = 0
                for i in X2:
                    if int(round(i)) != int(round(float(m) * 100)):
                        break
                    else:
                        count += 1

                # print("count: " + str(count))
                X2 = np.delete(X2, np.arange(count))
                F2 = np.delete(F2, np.arange(count))

                # print("---------------")
                plt.step(X2, F2, label="MANDATE=" + str(int(float(m) * 100)) + "%")

        plt.figure(1)
        plt.title("PREMIUM="+p)
        plt.legend()
        plt.minorticks_on()
        plt.grid(True, which='both')
        plt.xlabel("percent of games")
        plt.ylabel("loss")
        plt.tight_layout() 
        plt.savefig("figures/cdfs/pdf/PREMIUM=" + p + ".pdf")
        plt.savefig("figures/cdfs/png/PREMIUM=" + p + ".png")


        plt.figure(2)
        plt.title("PREMIUM="+p)
        plt.legend()
        plt.minorticks_on()
        plt.grid(True, which='both')
        plt.ylabel("percent of simulations")
        plt.xlabel("percent decrease in assets (mandated spending + losses from attacks)")
        # plt.xlim(0,105)
        # plt.ylim(60,100)
        # plt.xaxis.set_major_formatter(mtick.PercentFormatter(decimals=0))
        # plt.yaxis.set_major_formatter(mtick.PercentFormatter(decimals=0))
        plt.tight_layout() 
        plt.savefig("figures/old_cdfs/pdf/PREMIUM=" + p + ".pdf")
        plt.savefig("figures/old_cdfs/png/PREMIUM=" + p + ".png")


def total_loot_hist(df):
    # fig,a =  plt.subplots(2,5,sharey=True, sharex=True)

    plt.clf()
    fig = plt.figure(figsize=(4,2))
    a_win = fig.add_subplot(1,1,1)

    #neither_win = fig.add_subplot(1,3,3)

    mandates = sorted(df['MANDATE'].unique())
    for i,m in enumerate(mandates):
        #if (m == 1 or m == 0.9):
        if (m > 0.4):
            continue

        mands = df.loc[df['MANDATE'] == m]
        print(str(mands.to_numpy().size) + " is total size for this mandate")
        
        #Probe convergence conditions
        attacker_wins = mands.loc[mands['d_end'] == 0]
        defender_wins = mands.loc[mands['a_end'] == 0]   
        neither_wins = mands.loc[mands['d_end'] != 0]

        
        #convergences = mands['final_iter'].to_numpy()
        
        a_convergences = attacker_wins['final_iter'].to_numpy()
        d_convergences = defender_wins['final_iter'].to_numpy()
        n_convergences = neither_wins['final_iter']
        print("Neither wins: " + str(n_convergences.to_numpy().size) + " scenarios")
        
        print("Defenders totally looted in " + str(a_convergences.size) + " scenarios")
        print("Attackers completely looted in " + str(d_convergences.size) + " scenarios")

        print("Total: " + str(a_convergences.size + d_convergences.size + n_convergences.size) + " simulations")
        
        #Commenting this out, I think we need to show the 50 iter min -- ryan
        #convergences = convergences - 50

        N = a_convergences.size
                
        #Good for CDF, but we are changing to PDF
        #X1 = np.sort(a_convergences)
        #F1 = np.array(range(N))/float(N) * 100
        
        binsize = 50
        round_vals = [int(val/binsize) * binsize for val in a_convergences]
        (vals, counts) = np.unique(round_vals, return_counts=True)
        X1 = vals
        F1 = counts
        
        # X2 = np.sort(d_convergences)
        # F2 = np.array(range(N))/float(N) * 100
        
        # X3 = np.sort(n_convergences)
        # F3 = np.array(range(N))/float(N) * 100

        a_win.plot(X1, F1, label=str(int(m * 100)) + "%", linewidth=2)
        #d_win.step(X2, F2, label=str(int(m * 100)) + "%")
        #neither_win.step(X3, F3, label=str(int(m * 100)) + "%")

    plt.ylim(0, 375)
    # plt.title("PDF of simulation iterations when attackers win")
    #ax.yaxis.set_major_formatter(mtick.PercentFormatter(decimals=0))
    # plt.yscale("log")
    plt.xlim(left=0, right=600)
    plt.minorticks_on()
    plt.grid(True, which='both')
    plt.xlabel("Duration of games (iterations)")
    plt.ylabel("Number of games")
    plt.legend(loc="upper right", title="MANDATE:", fancybox=True, shadow=True, ncol=1)
    plt.tight_layout()
    # plt.show() 
    plt.savefig("figures/total_loot.pdf", pad_inches=0)

def multi_sweep(df):
    
    sweep_vars = ['PAYOFF', 'EFFORT', 'EFFICIENCY']#, 'SEC_INVESTMENT', 'PERCENT_EVIL', 'WEALTH_GAP']
    # param_names = ['PAYOFF', 'EFFORT', 'EFFICIENCY', 'MANDATE', 'ATTACKERS', 'INEQUALITY']
    # human_readable = ['PAYOFF', 'EFFORT', 'EFFICIENCY', 'investment', 'number of attackers', 'inequality']

    si_labels = np.arange(0, 1.1, 0.1).tolist()
    si_nums = [round(num,1) for num in si_labels]
    # rounded_labels = [(str(int(float(num) * 100)) + "%") for num in si_nums]

    # si_xvals = [round(num, 2) for num in si_labels]

    norm_vals = np.arange(0.1, 1.1, 0.1).tolist()
    xvals = [round(num,2) for num in norm_vals]
    print(xvals)


    for sweep_var in sweep_vars:

        plt.clf()
        fig = plt.figure(figsize=(4,2.5))
        ax = fig.add_subplot(1,1,1)
        

        for num in range(0, 8):
            sweep_graph = []
            #another array to store total percent loss relative to original assets
            total_graph = []
                
            dframe = df.loc[df["MANDATE"] ==  str(si_nums[num])]
            # if sweep_var == 'SEC_INVESTMENT':
            #     dframe = pd.read_csv(base_path + sweep_var  + '.csv', index_col=False, header=0)
            
            i=0
            for payoff_val in dframe[sweep_var]:
                #make relative
                #sweep_graph.append((dframe['d_init'][i] - dframe['d_end'][i]) / (dframe['d_init'][i]))
                if dframe['d_init'][i] != 0:
                    sweep_graph.append(((dframe['d_init'][i]/(1 - dframe['MANDATE'][i])) - dframe['d_end'][i] ) / (dframe['d_init'][i]/(1 - dframe['MANDATE'][i])))
                else:
                    sweep_graph.append(1)
                i+=1
            
            sweep_graph = np.array(sweep_graph) * 100

            ax.plot(xvals, sweep_graph, label=str(num*10) + "%")
            ax.yaxis.set_major_formatter(mtick.PercentFormatter())

                
            
            handles, labels = ax.get_legend_handles_labels()
            # print(handles)
            # print(labels)
            # print(rounded_labels)
            # print(list(reversed(rounded_labels)))
            handles, labels = ax.get_legend_handles_labels()

            plt.grid(True, which='both')
            # plt.title("Total inclusive relative defender losses vs. " + human_readable[sweep_vars.index(sweep_var)])

        print(sweep_var)

        plt.legend(reversed(handles), reversed(labels), title="MANDATE:", bbox_to_anchor=(1.3,1), loc="upper right", fancybox=True, shadow=True, ncol=1)
        plt.xlabel(sweep_var)
        # plt.xticks(si_nums)
        plt.ylabel("Total Defender losses (%)")
        plt.ylim(0,100)
        plt.xlim(0.1,1.1)
        plt.tight_layout()
        plt.savefig('figures/' + sweep_var +  '_MANDATE_sweep.pdf',bbox_inches='tight')


# Sanity checks to make sure model matches 
def main():
    filename = "../logs/test_no_gov.csv"
    print("loading " + filename)
    df = pd.read_csv(filename)
    # print(df.info())

    # make_cdfs(df) # uncomment to run
    total_loot_hist(df)
    multi_sweep(df)



if __name__=="__main__":
    main()




