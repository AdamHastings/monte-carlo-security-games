from multiprocessing import Pool
import itertools
import numpy as np

def func(a,b,c,d,e,f):
    print(a,b,c,d,e,f)


def permutations():
    pass

if __name__ == '__main__':
    
    inputs = [] # [(0, 0, 0), (0, 0, 1), (0, 1, 0), ... (2, 2, 2)]

    a = np.linspace(0.1, 1.0, 10)
    b = np.linspace(0.1, 1.0, 10)
    c = np.linspace(0.1, 1.0, 10)
    d = np.linspace(0.1, 1.0, 10)
    e = np.linspace(0.1, 1.0, 10)
    f = np.linspace(0.1, 1.0, 10)
    # print(a)
    # print("-------")

    inputs = list(itertools.product(a,b,c,d,e,f))
    # print(inputs)
    
    with Pool(5) as p:
        p.starmap(func, inputs)