#!/usr/bin/python
import datetime
import os
import sys
import subprocess

''' A batch run of memaslap.

Memaslap specifics are setup in the EBBRT_BATCH_CMD variable
'''

if len(sys.argv) != 3:
    print "Usage: <test-name> <runs>"
    exit(0)
jobname = sys.argv[1]
jobruns = int(sys.argv[2])
cmd = ''
try: 
    cmd = os.environ['EBBRT_BATCH_CMD']
except KeyError:
    print "cmd variable not found"
    exit(0)

jobfile = os.path.join(os.getcwd(),jobname)
joblog = jobfile+"_log" 
jobstat = jobfile+"_stat"
if os.path.isfile(joblog) or os.path.isfile(jobstat):
    print "A test with this name already exists."
    # todo: allow overwrite
    print "Aborting..."
    exit(0)

buf = '\t'
with open(joblog, 'w') as log:
    with open(jobstat, 'w') as stats:
        #appead date/time and command to file
        log.write("# "+cmd+"\n") 
        log.write("# "+str(datetime.datetime.now())+"\n")
        log.flush()
        stats.write("#"+jobname)
        # execute tests
        for x in range(0, jobruns):
            output = subprocess.check_output(cmd, shell=True)
            log.write(output+"\n") 
            lines = output.split('\n')
            lline = lines[len(lines)-2].split(' ')
            if not (lline[0] == "Run"):
                print "An error has occured with the test"
                exit(0)
            stats.write(lline[2][:-1]+buf)
            stats.write(lline[6]+buf)
            stats.write(lline[8][:-3]+buf)
            stats.write('\n')
            stats.flush()

print "#end"
