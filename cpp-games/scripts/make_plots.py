#!/usr/bin/python3
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

CAUGHT_range = 11
CLAIMS_range = 10

def make_insurance_plots():
    caught_df = pd.read_csv('parse_results/trimmed/insurance_hist_items_CAUGHT.csv')
    # print(caught_df)
    caught_df = caught_df[~(caught_df['count'] < 100)]
    # print(caught_df)

    plt.clf()
    
    CAUGHT_duration_fig, CAUGHT_duration_ax = plt.subplots()
    CAUGHT_loss_fig, CAUGHT_loss_ax = plt.subplots()
    
    for i in range(CAUGHT_range):
        Xs = caught_df[caught_df['CAUGHT_idx'] == i]['lowval']
        # print(duration_Xs)
        duration_Ys = caught_df[caught_df['CAUGHT_idx'] == i]['avg_duration']
        loss_Ys     = caught_df[caught_df['CAUGHT_idx'] == i]['avg_loss']

        label = round(i/10, 2)
        CAUGHT_duration_ax.plot(Xs, duration_Ys, label=label)
        CAUGHT_loss_ax.plot(Xs, loss_Ys, label=label)

    CAUGHT_duration_fig.legend(title="CAUGHT=", fancybox=True, shadow=True,)
    CAUGHT_duration_ax.set_ylabel("Average trial duration")
    CAUGHT_duration_ax.set_xlabel("Insurer initial assets")
    # CAUGHT_duration_ax.hlines(CAUGHT_duration_ax.get_yticks(), xmin=0, xmax=10)
    CAUGHT_duration_fig.savefig("figures/insurance/CAUGHT_duration_fig.png")

    # CAUGHT_loss_fig.legend(loc='best',title="Game parameter:", bbox_to_anchor=(1.0, 0.6, 0.3, 0.5), fancybox=True, shadow=True, ncol=1)
    CAUGHT_loss_fig.legend(title="CAUGHT=", fancybox=True, shadow=True,)
    CAUGHT_loss_ax.set_ylabel("Average loss")
    CAUGHT_loss_ax.set_xlabel("Insurer initial assets")
    CAUGHT_loss_fig.savefig("figures/insurance/CAUGHT_loss_fig.png")
    # plt.clf()
    

    

def main():
    # plot something
    plt.plot([1, 2, 3], [4, 5, 6])
    plt.title('My plot')

    # save the plot to a file
    plt.savefig('my_plot.png')

    make_insurance_plots()


if __name__=="__main__":
    main()
