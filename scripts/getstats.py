#!/usr/bin/python
import csv
import sys
import time
import numpy as n

cols = []
ncols = []
numcols = 0

#read cols from stdin
try:
    for line in sys.stdin:
      if not line[0] == '#':
        v = line.split('\t')
        if numcols == 0:
            numcols = len(v) - 1 #minus removes newline
            for i in range(numcols):
                cols.append([])
        for i in range(numcols):
            cols[i].append(v[i])
except KeyboardInterrupt:
    sys.stdout.flush()
    pass

print "order: min,max,mean,var,std";
for i in range(numcols):
    ncols.append(n.array(cols[i], float))
    csv.writer(sys.stdout).writerow([ncols[i].min(), ncols[i].max(),
        ncols[i].mean(), ncols[i].var(), ncols[i].std()])
