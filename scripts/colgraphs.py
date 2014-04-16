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



    Structure:

    This script` will generate 5 plots for series of trials. You can add
    additional series to the graphs by including them in stdin with the
    appropriate marker. 
    
    
'''
import csv
import sys
import time
import numpy as n
import matplotlib.pyplot as plt
import matplotlib as mpl

init_series = False
init_trial = False

graphs = ["Average", "Min", "Max"] # TODO: graph-specific lables/colors goes here
graph_data = [] 
for i in range(len(graphs)):
  graph_data.append([]) # graph topic

series = [] # series labels (i.e., Ebbrt, Linux)
trials = [] # trial labels (i,e., 32, 64, 128)
trial_raw = [] # structure for raw data 
"""
   graph_data = [ topic: [ series: [ column: [0,0,0]]]
   trial_raw =  [ series: [ column: [t1[],t2[],t3[]]]
"""
series_ctr = 0
trial_ctr = 0
column_ctr = 0

# trial data

# Step 2, read input 
try:
  for line in sys.stdin:
    #initialization steps
    if init_series == False and line[:2] != "##":
      sys.exit("Error: first line of input does not signal new series")
    elif init_trial == False and line[:1] != "#":
      sys.exit("Error: second line of input does not signal new trial")

    # setup series structures
    if line[:2] == "##":
      series_ctr += 1
      column_ctr = 0 
      trial_ctr = 0
      series.append(line[2:-1])
      trials.append([])
      trial_raw.append([])
      # append a series structure for each graph 
      for i in range(len(graph_data)):
        graph_data[i].append([]) 
      if init_series == False:
        init_series = True

    # setup trial structures
    elif line[0] == "#":
      trial_ctr += 1
      trials[series_ctr-1].append(line[1:-1])
      # append a trial structure for each graph 
      if init_trial == False:
        init_trial = True

    # parse data line
    else:
      nline = line[:-1]
      cols = nline.split('\t') 
      ###
      if column_ctr == 0:  
        column_ctr = len(cols)
        for i in range(column_ctr):
          trial_raw[series_ctr-1].append([])
          # extend graph data for column
          for i in range(len(graphs)):
            graph_data[i][series_ctr-1].append([]) 
      for i in range(column_ctr):
        try:
          trial_raw[series_ctr-1][i][trial_ctr-1].append(cols[i])
        except IndexError:
          trial_raw[series_ctr-1][i].append([])
          trial_raw[series_ctr-1][i][trial_ctr-1].append(cols[i])
except KeyboardInterrupt:
    sys.stdout.flush()
    pass

print trials

# Step 3, gather statistics
for s in range(len(trial_raw)):
  for c in range(len(trial_raw[s])):
    # column loop
    for t in range(len(trial_raw[s][c])):
      narray = n.array(trial_raw[s][c][t], float)
      graph_data[0][s][c].append(narray.mean())
      graph_data[1][s][c].append(narray.min())
      graph_data[2][s][c].append(narray.max())
      
# DEBUG
#print graphs
#print series
#print trials
#print graph_data
#print trial_raw

# Step 4, build graphs
###### Averages Graph
for g in range(len(graph_data)):
  for s in range(len(graph_data[g])):
    fig = plt.figure()
    ax = fig.add_subplot(111)
    ax.semilogx( trials[s], graph_data[0][s][0])
    ax.xaxis.get_major_locator().base(2)
    ax.xaxis.get_minor_locator().base(2)
    ax.xaxis.get_minor_locator().subs([1.5])
    ax.xaxis.set_major_formatter(mpl.ticker.ScalarFormatter())
    plt.show()

