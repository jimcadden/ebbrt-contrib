#!/usr/bin/python
import datetime
import os
import sys
import math
import time
import subprocess
### Python 2.6 hack
if "check_output" not in dir( subprocess ): # duck punch it in!
    def f(*popenargs, **kwargs):
        if 'stdout' in kwargs:
            raise ValueError('stdout argument not allowed, it will be overridden.')
        process = subprocess.Popen(stdout=subprocess.PIPE, *popenargs, **kwargs)
        output, unused_err = process.communicate()
        retcode = process.poll()
        if retcode:
            cmd = kwargs.get("args")
            if cmd is None:
                cmd = popenargs[0]
            print cmd
            raise subprocess.CalledProcessError(retcode,cmd)
        return output
    subprocess.check_output = f
#####
''' 
    EBBRT_MEMASLAP_CMD  - base command that will be augmented with each trial.
                          Use the placeholder "{IP}" 
    EBBRT_VMBOOT_CMD - script to bring up vm, return IP
    EBBRT_VMCLEAN_CMD - TODO...
'''

if len(sys.argv) != 4:
    print "Usage: <experiment-name> <experiment-type> <num-trials>"
    exit(0)

exp_name = sys.argv[1]
exp_type = sys.argv[2]
trial_count = sys.argv[3]
exp_logs = os.path.join(os.getcwd(),exp_name+'.log')
exp_cols = os.path.join(os.getcwd(),exp_name+'.data')

end_exp = False
i = 0

def cmd_flag(test, count):
    if test == "concurrency" or test == "concurrency-quick":
      return "-c"
    elif test == "payload" or test =="payload-quick" or test=="payload-single-mtu":
      return "-X"
    elif test == "singleton":
      return ""
    else:
      print "error: unknown experiment type", test
      exit(0)

def cmd_val(test, count):
    if test == "concurrency":
      if count == "0":
        return 4
      return str(math.pow(2,count+2)) #4.8.16.32...
    if test == "concurrency-quick":
      return str((4 * (count+1) * (count+1) * (count+1)))
    elif test == "payload-quick":
      return str((500 * (count+1) * (count+1)))
    elif test=="payload-single-mtu":
      return str((64 * (count+1) * (count+1)))
    elif test=="payload":
      if count == "0":
	return 64
      else: 
	return str((count)*100) #100,200,300
    elif test == "singleton":
      return ""
    else:
      print "error: unknown experiment type", test
      exit(0)

def continue_exp(test, count):
    if test == "concurrency":
      return (count < 8) # (2^9) = 512
    elif test == "concurrency-quick":
      return (count < 5) #  
    elif test == "payload-quick":
      return (count < 10) #  
    elif test == "payload-single-mtu":
      return (count < 4) # 64 * 4^2 = 1024 
    elif test == "payload":
      return (count < 30) 
    elif test == "singleton":
      return (count < 1)
    else:
      print "Error: unknown experiment type", test
      exit(0)

def build_cmd(test, count):
    ip = subprocess.check_output(vm_boot_cmd, shell=True)
    ip_clean = ip.rstrip('\r\n')
    if not ip_clean:
      sys.stderr.write('Error, we were unable to get an IP after boot.\n')
      exit(0)
    retstr = str(base_cmd+" "+str(cmd_flag(test, count))+" "+str(cmd_val(test,count)))
    return retstr.replace("{IP}", ip_clean) 


if os.path.isfile(exp_logs) or os.path.isfile(exp_cols):
    print "An experiment in this directory with this name already exists."
    print "Aborting..."
    exit(0)

#######################################################################

try: 
    base_cmd = os.environ['EBBRT_MEMASLAP_CMD'] 
    if not base_cmd:
      raise KeyError()
except KeyError:
    print "Missing envoirnment variable: EBBRT_MEMASLAP_CMD"
    print "Aborting..."
    exit(0)
try: 
    vm_boot_cmd = os.environ['EBBRT_VMBOOT_CMD']
    if not vm_boot_cmd:
      raise KeyError()
except KeyError:
    print "Missing envoirnment variable: EBBRT_VMBOOT_CMD"
    print "Aborting..."
    exit(0)



with open(exp_logs, 'w') as log:
  with open(exp_cols, 'w') as data:
#write one : exp name, type and date
    log.write("# "+exp_name+" "+exp_type+" "+trial_count+"\n")
    log.write("# "+str(datetime.datetime.now())+"\n")
    log.write("# "+str(vm_boot_cmd)+"\n")
    data.write("##"+exp_name+"\n") 
    log.flush()
    data.flush()
    while continue_exp(exp_type, i):
      # for each experiment stage
      cmd = build_cmd(exp_type, i)
      sys.stderr.write(cmd)
      log.write("#"+cmd+'\n')
      data.write("#"+cmd_val(exp_type, i)+"\n") 
      log.flush()
      data.flush()
      # execute many trials of experiment
      for x in range(0, int(trial_count)):
        print cmd
        output = subprocess.check_output(cmd, shell=True)
        print output
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
        log.flush()
        data.flush()
      i += 1 # next test
      time.sleep(3)
print "## END EXPERIMENT"
    
