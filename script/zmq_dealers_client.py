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

if os.path.isdir('results/zmq_result_%s'%tag): 
    pass 
else: 
    os.mkdir('results/zmq_result_%s'%tag)

node_num = 20

if len(sys.argv) > 1:
   node_num = sys.argv[1]

for i in range(1, node_num) :
    os.system('nohup bin/zmq_dealer_client %s > results/zmq_result_%s/client_%s &'%(i, tag, i))
