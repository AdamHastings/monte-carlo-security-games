#!/usr/bin/python3
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

CAUGHT_range = 11
CLAIMS_range = 10

def make_CAUGHT_insurance_plots():
    plt.clf()
    caught_df = pd.read_csv('parse_results/trimmed/insurance_hist_items_CAUGHT.csv')
    
    caught_df = caught_df[~(caught_df['count'] < 100)]
    

    
    CAUGHT_duration_fig, CAUGHT_duration_ax = plt.subplots()
    CAUGHT_loss_fig, CAUGHT_loss_ax = plt.subplots()
    
    for i in range(CAUGHT_range):
        Xs = caught_df[caught_df['CAUGHT_idx'] == i]['lowval']
        duration_Ys = caught_df[caught_df['CAUGHT_idx'] == i]['avg_duration']
        loss_Ys     = caught_df[caught_df['CAUGHT_idx'] == i]['avg_loss']

        label = round(i/10, 2)
        CAUGHT_duration_ax.plot(Xs, duration_Ys, label=label)
        CAUGHT_loss_ax.plot(Xs, loss_Ys, label=label)

    CAUGHT_duration_fig.legend(title="CAUGHT=", fancybox=True, shadow=True,)
    CAUGHT_duration_ax.set_ylabel("Average trial duration")
    CAUGHT_duration_ax.set_xlabel("Insurer initial assets")
    CAUGHT_duration_fig.savefig("figures/insurance/CAUGHT_duration_fig.png")

    CAUGHT_loss_fig.legend(title="CAUGHT=", fancybox=True, shadow=True,)
    CAUGHT_loss_ax.set_ylabel("Average loss")
    CAUGHT_loss_ax.set_xlabel("Insurer initial assets")
    CAUGHT_loss_fig.savefig("figures/insurance/CAUGHT_loss_fig.png")
    
def make_CLAIMS_insurance_plots():
    plt.clf()
    claims_df = pd.read_csv('parse_results/trimmed/insurance_hist_items_CLAIMS.csv')
    
    claims_df = claims_df[~(claims_df['count'] < 100)]
    

    
    CLAIMS_duration_fig, CLAIMS_duration_ax = plt.subplots()
    CLAIMS_loss_fig, CLAIMS_loss_ax = plt.subplots()
    
    for i in range(CLAIMS_range):
        Xs = claims_df[claims_df['CLAIMS_idx'] == i]['lowval']
        duration_Ys = claims_df[claims_df['CLAIMS_idx'] == i]['avg_duration']
        loss_Ys     = claims_df[claims_df['CLAIMS_idx'] == i]['avg_loss']

        label = round(i/10 + 0.1, 2)
        CLAIMS_duration_ax.plot(Xs, duration_Ys, label=label)
        CLAIMS_loss_ax.plot(Xs, loss_Ys, label=label)

    CLAIMS_duration_fig.legend(title="CLAIMS=", fancybox=True, shadow=True,)
    CLAIMS_duration_ax.set_ylabel("Average trial duration")
    CLAIMS_duration_ax.set_xlabel("Insurer initial assets")
    CLAIMS_duration_fig.savefig("figures/insurance/CLAIMS_duration_fig.png")

    CLAIMS_loss_fig.legend(title="CLAIMS=", fancybox=True, shadow=True,)
    CLAIMS_loss_ax.set_ylabel("Average loss")
    CLAIMS_loss_ax.set_xlabel("Insurer initial assets")
    CLAIMS_loss_fig.savefig("figures/insurance/CLAIMS_loss_fig.png")

def make_insurance_plots():
    make_CAUGHT_insurance_plots()
    make_CLAIMS_insurance_plots()
    
    

    

def main():
    # plot something
    plt.plot([1, 2, 3], [4, 5, 6])
    plt.title('My plot')

    # save the plot to a file
    plt.savefig('my_plot.png')

    make_insurance_plots()


if __name__=="__main__":
    main()
