#!/usr/bin/env python3
import os

for i in range(1, 11):
	print("running relative sweep to MANDATE...")
	print(str(i) + "/10")
	os.system("./sweep_filter.sh " + str(float(i/10)))
		
