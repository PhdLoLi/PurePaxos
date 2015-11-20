import os
import random
import sys
from datetime import datetime, date, time
from time import sleep
tag = datetime.now().strftime("%m-%d-%H:%M")


if os.path.isdir('results'):
    pass
else:
    os.mkdir('results')

if os.path.isdir('results/evaluation_others_%s' % tag): 
    pass 
else: 
    os.mkdir('results/evaluation_others_%s' % tag)

node_num = 20

if len(sys.argv) > 2:
   start = int(sys.argv[1])
   node_num = int(sys.argv[2])

for i in range(start, node_num) :
    os.system('nohup bin/evaluation_others %d %d > results/evaluation_others_%s/other_%d &'%(i, node_num, tag, i))
