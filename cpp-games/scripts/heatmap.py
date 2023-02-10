#!/usr/bin/python3
import time 
import random
import sys


# def random_line(afile):
#     line = next(afile)
#     for num, aline in enumerate(afile, 2):
#         if random.randrange(num):
#             continue
#         line = aline
#     return line


# filename = '../logs/MANDATE=0.0.csv'
# with open(filename, 'r') as f:
#     for _ in range(100):
#         print(random_line(f))

# sys.exit(0)



logrange = [0.0, 0.1, 0.3, 0.4]



param1 = 'TAX'
param2 = 'PREMIUM'

for l in logrange:
    filename = '../logs/MANDATE=' + str(l) + '.csv'
    ctr = 0
    with open(filename) as f:
        # for line in f:
        for count, line in enumerate(f, start=1):
            print(count)
            count += 10000
            continue
        
            # ctr += 1
            # if ctr % 1000000 == 0:
            #     print(ctr)

end =  time.time()