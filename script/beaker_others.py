import os
import random
import sys
from datetime import datetime, date, time
from time import sleep
tag = datetime.now().strftime("%m-%d-%H:%M")

d_path = "config/nodes-5.yaml"
if os.path.isdir('results'):
    pass
else:
    os.mkdir('results')

if os.path.isdir('results/beaker_others_%s'%tag): 
    pass 
else: 
    os.mkdir('results/beaker_others_%s'%tag)

node_num = 5 

if len(sys.argv) > 1:
   node_num = sys.argv[1]
if len(sys.argv) > 2:
   d_path = sys.argv[2]

for i in range(1, int(node_num)) :
    os.system('nohup bin/process_others %s %s > results/beaker_others_%s/client_%s &'%(i, d_path, tag, i))
