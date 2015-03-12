#!/usr/bin/python
"A dump script to prepend filename to an existing (log) file"

import os
import sys

if len(sys.argv) != 2:
    print "Usage: <directory path>"
    exit(0)

for file_path in os.listdir(sys.argv[1]):
       f = open(file_path, 'r'); s = f.read(); f.close()
       l = s.splitlines(); l.insert(0, "#"+file_path); s = '\n'.join(l)
       f = open(file_path, 'w'); f.write(s); f.close()
