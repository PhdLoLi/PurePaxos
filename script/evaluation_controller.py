import zmq
import sys
import threading
import time
from random import randint, random
def main():
    """main function"""

    context = zmq.Context()
    server = context.socket(zmq.ROUTER)
    server.bind('tcp://*:5570')

    poll = zmq.Poller()
    poll.register(server, zmq.POLLIN)

    while True:
        sockets = dict(poll.poll())
        if server in sockets:
            ident, msg = server.recv_multipart()
            print('Server received %s id %s' % (msg, ident))
            server.send_multipart([ident, msg])

    server.close()
    context.term()

if __name__ == "__main__":
    main()
