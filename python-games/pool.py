from multiprocessing import Pool
import itertools
import numpy as np


globalvar = 0

def func(a,b,c,d,e,f):
    print(a,b,c,d,e,f)

def func2(a,b):
    # globalvar += a
    local = globalvar
    local += a
    print(a,b, local)

if __name__ == '__main__':
    
    inputs = [] # [(0, 0, 0), (0, 0, 1), (0, 1, 0), ... (2, 2, 2)]

    a = np.linspace(0.0, 1.0, 5)
    b = np.linspace(0.0, 1.0, 5)
    c = np.linspace(0.1, 1.0, 10)
    d = np.linspace(0.1, 1.0, 10)
    e = np.linspace(0.1, 1.0, 10)
    # f = [5]
    # print(a)
    # print("-------")

    # inputs = list(itertools.product(a,b,c,d,e,f))
    inputs = list(itertools.product(a,b))
    # print(inputs)
    
    with Pool() as p:
        # p.starmap(func, inputs)
        p.starmap(func2, inputs)