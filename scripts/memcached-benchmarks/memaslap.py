#!/usr/bin/python
import datetime
import os
import sys
import subprocess
''' 
    EBBRT_MEMASLAP_CMD  - base command that will be augmented with each trial
'''

if len(sys.argv) != 4:
    print "Usage: <experiment-name> <experiment-type> <num-trials>"
    exit(0)

exp_name = sys.argv[1]
exp_type = sys.argv[2]
trial_count = sys.argv[3]
exp_logs = os.path.join(os.getcwd(),exp_name+'.log')
exp_cols = os.path.join(os.getcwd(),exp_name+'.data')
exp_cmd = os.environ['EBBRT_MEMASLAP_CMD'] 
end_exp = False
i = 0

def cmd_flag(test, count):
    if test == "concurrency":
      return "-c"
    elif test == "singleton":
      return ""
    else:
      print "error: unknown experiment type", test
      exit(0)

def cmd_val(test, count):
    if test == "concurrency":
      return str((4 * (count+1) * (count+1)))
    elif test == "singleton":
      return ""
    else:
      print "error: unknown experiment type", test
      exit(0)

def build_cmd(test, count):
    retstr = " "+str(cmd_flag(test, count))+" "+str(cmd_val(test,count))
    return retstr

def continue_exp(test, count):
    if test == "concurrency":
      return (count < 13) # 4 *(13^2) = 676
    elif test == "singleton":
      return (count < 1)
    else:
      print "Error: unknown experiment type", test
      exit(0)

if os.path.isfile(exp_logs) or os.path.isfile(exp_cols):
    print "An experiment in this directory with this name already exists."
    print "Aborting..."
    exit(0)
try: 
    trial_cmd = os.environ['EBBRT_MEMASLAP_CMD']
except KeyError:
    print "Missing envoirnment variable: EBBRT_MEMASLAP_CMD"
    print "Aborting..."
    exit(0)

with open(exp_logs, 'w') as log:
  with open(exp_cols, 'w') as data:
#write one : exp name, type and date
    log.write("#"+exp_name+" "+exp_type+" "+trial_count)
    log.write("# "+str(datetime.datetime.now())+"\n")
    data.write("##"+exp_name+"\n") 
    log.flush()
    while continue_exp(exp_type, i):
      # write on each test
      aug_str = build_cmd(exp_type, i)
      cmd = exp_cmd+" "+aug_str
      log.write("#"+exp_name+" "+aug_str+'\n')
      data.write("#"+cmd_val(exp_type, i)+"\n") 
      # execute many trials of experiment
      for x in range(0, int(trial_count)):
        output = subprocess.check_output(cmd, shell=True)
        log.write(output+"\n") 
        lines = output.split('\n')
        lline = lines[len(lines)-2].split(' ')
        if not (lline[0] == "Run"):
          print "Unable to parse the Memaslap output, aborting experiment. "
          exit(0)
        data.write(lline[2][:-1]+'\t')
        data.write(lline[6]+'\t')
        data.write(lline[8][:-3])
        data.write('\n')
        data.flush()
      i += 1 # next test

print "## END EXPERIMENT"
    
