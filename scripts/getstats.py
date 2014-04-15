#!/usr/bin/python
''' Input format must be as follows: 

# Trial 123
12 33  442.5 595   3343.55
5  13  42.33  54495 3243.44
12 31  442.5 595   3343.55

    - Comment identifies the start of a data set
    - Script can support and number of tab-seperated columns
      Values are assumed to be floats
    - Statistics are output in CSV format, each row containing
      the results for all columns of a trial. Column data is seperated by an empty field.
    - Order of results min,max,mean,var,std
'''
import csv
import sys
import time
import numpy as n
cols = []
ncols = []
numcols = 0
title = ""
in_trial = False

try:
  for line in sys.stdin:
    if in_trial == True:
      if line[0] == "#":
        # signals the start of another trial, lets dump previous data
        writerlist = [title]
        for i in range(numcols):
            ncols.append(n.array(cols[i], float))
            writerlist += [ncols[i].min(), ncols[i].max(),
                ncols[i].mean(), ncols[i].var(), ncols[i].std(), " "]
            sys.stdout.flush()
        csv.writer(sys.stdout).writerow(writerlist)
        # reset trial variables 
        numcols = 0
        cols[:] = []
        ncols[:] = []
        title = line[:-2] # remove newline char
      elif not line == "\n": # skip blank lines
        v = line.split('\t')
        if numcols == 0:
          numcols = len(v) - 1 #minus removes newline
          for i in range(numcols):
              cols.append([])
        for i in range(numcols):
          cols[i].append(v[i])
    else: # not yet in trial
      if line[0] == "#":
        title = line[:-2] # remove newline char
        in_trial = True
except KeyboardInterrupt:
    sys.stdout.flush()
    pass

# a final dump
writerlist = [title]
for i in range(numcols):
    ncols.append(n.array(cols[i], float))
    writerlist += [ncols[i].min(), ncols[i].max(),
        ncols[i].mean(), ncols[i].var(), ncols[i].std(), " "]
    sys.stdout.flush()
csv.writer(sys.stdout).writerow(writerlist)
