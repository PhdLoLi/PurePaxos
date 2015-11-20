import subprocess
import sys
import os
import threading
import zmq
import time
from threading import Timer
import ppaxos_pb2

node_id = 0
node_size = 3
value_size = 1
win_size = 1 

class ServerProposer(threading.Thread):
    """ServerProposer"""
    def __init__(self):
        threading.Thread.__init__ (self)

    def run(self):
        os.system('killall evaluation_X_Y')
        os.system('bin/evaluation_X_Y %d %d %d %d > X_Y'%(node_id, node_size, value_size, win_size))

class ServerController(threading.Thread):
    """ServerController"""
    def __init__(self):
        threading.Thread.__init__ (self)
        self.throughputs = {}
        self.protoResults = {}
        self.finish = 0
        self.clients = []

    def timeout(self):
        print "Game over"
        self.finish = 1

        msg = "Terminate!"

        for i in range(node_size):
            self.server.send_multipart([self.clients[i], msg])
        print("Send Terminating Msg Finish")
        sys.exit()

    def run(self):
        print("Controller Start")
        context = zmq.Context()
        self.server = context.socket(zmq.ROUTER)
        self.server.bind('tcp://*:5571')

        poll = zmq.Poller()
        poll.register(self.server, zmq.POLLIN)

        counter = 0

        while True:
            sockets = dict(poll.poll())
            if self.server in sockets:
                ident, msg = self.server.recv_multipart()
                print('Server Controller received %s id %s' % (msg, ident))
                counter = counter + 1
                self.clients.append(ident)
                self.throughputs[ident] = []
                self.protoResults[ident] = []

            if counter == node_size:
                msg = "Init OK!"
                print("Receive from %s !"%node_size)
                for i in range(node_size):
                    self.server.send_multipart([self.clients[i], msg])
                time.sleep(15)

                msg = "Start Count!"
                print("Send Starting Count Command")

                for i in range(node_size):
                    self.server.send_multipart([self.clients[i], msg])
                break

        t = Timer(30, self.timeout)
        t.start()

        while True:
            print("Inside me")

            if self.finish == 1 :
                break

            sockets = dict(poll.poll())
            if self.server in sockets:
                ident, msg = self.server.recv_multipart()
                print('Server Controller received Evaluation Data from id %s' % (ident))
                
                result = ppaxos_pb2.MsgResult()
                result.ParseFromString(msg)
                print(result)
                self.throughputs[ident].append(result.throughput) 
                self.protoResults[ident].append(result) 

        print("I'm finishing")
        self.server.close()
        context.term()

        for key in self.protoResults:
          print key
          times = 1;
          for res in self.protoResults[key]:
            print "numbers -- ", times
            print "time : ", 30 + times * 5, "seconds"
            print "throughput in 5 second -- ", res.throughput  
            for ptt in res.msg_ptt:
              print "------------------------------------"
              print "percent -- ", ptt.percent, " %"
              print "period -- ", ptt.period, " ns"
              print "exe_period -- ", ptt.exe_period, " ns"
              print "try_time -- ", ptt.try_time
            times = times + 1
            print "--*--*--*--*--*--*--*--*--*--*--*--"

        print(self.throughputs)

        print "-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-"

        print "node_size: %d value_size: %d win_size: %d" % (node_size, value_size, win_size)
          

def main():
    """main function"""
    global node_id, value_size, node_size, win_size

    if len(sys.argv) > 4 :
        node_id = int(sys.argv[1]) 
        node_size = int(sys.argv[2])
        value_size = int(sys.argv[3])
        win_size = int(sys.argv[4])
    else:
        print "no input node_id node_size value_size win_size"
        print "use default 0 3 1 1"

    proposer = ServerProposer()
    controller = ServerController()

    HOST="msmummy@192.168.5.21"
    # Ports are handled in ~/.ssh/config since we use OpenSSH
    COMMAND="cd /home/msmummy/ppaxos" + ";" \
             "killall evaluation_others" + ";" \
             "python script/evaluation_others.py %d %d" % (node_id + 1, node_size)
    
    ssh = subprocess.Popen(["ssh", "%s" % HOST, COMMAND],
                           shell=False,
                           stdout=subprocess.PIPE,
                           stderr=subprocess.PIPE)
    result = ssh.stdout.readlines()
    if result == []:
            error = ssh.stderr.readlines()
            print >>sys.stderr, "ERROR: %s" % error
    else:
            print result
    

    print("Here")
#    os.system('killall evaluation_X_Y')
#    os.system('bin/evaluation_X_Y 0 3 1 1')
    proposer.start()
    controller.run()
#    controller.join()
    proposer.join()
    print("Over")

if __name__ == "__main__":
    main()
