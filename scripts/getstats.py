#!/usr/bin/python
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

for i in range(numcols):
    ncols.append(n.array(cols[i], float))
    print ncols[i].min()
    print ncols[i].max()
    print ncols[i].mean()
    print ncols[i].var()
    print ncols[i].std()
    print "#"
#print ncol1.mean(), ncol1.var(),ncol1.std(),ncol1.min(),ncol1.max()
