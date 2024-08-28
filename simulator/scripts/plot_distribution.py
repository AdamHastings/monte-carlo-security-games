import matplotlib.pyplot as plt 
import matplotx
import numpy as np
import os
import sys
import json


def plot_distribution(filename, samples):

    with plt.style.context(matplotx.styles.dufte):

        plt.hist(samples, bins=100)

        plt.xlabel("initial wealth")
        plt.ylabel("count")
        # matplotx.ylabel_top("percentage")  # move ylabel to the top, rotate
        # matplotx.line_labels()  # line labels to the right
        # plt.xlim(0, 300)
        # plt.ylim(-0.1, 1.1)
        plt.tight_layout()


        path = "../scripts/figures/wealthdist"
        if not os.path.isdir(path):
            os.mkdir(path)

        plt.savefig(path + "/" + filename + ".png")
        plt.savefig(path + "/" + filename + ".pdf")



if __name__=="__main__":

    if len(sys.argv) == 3:
        # npa = np.asarray(someListOfLists, dtype=np.float32)
        sampledata = np.asarray(sys.argv[2].split(','), dtype=np.int64)
        # sampledata = json.loads(vector_str)  # Convert JSON string to Python list
        # print(sampledata)
        filename = sys.argv[1]
    else:
        sampledata = np.random.normal(100, 10, 1000)
        filename = "test"

    plot_distribution(filename, sampledata)