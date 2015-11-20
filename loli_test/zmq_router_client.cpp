//  Asynchronous client-to-server (DEALER to ROUTER)
//
//  While this example runs in a single process, that is to make
//  it easier to start and stop the example. Each task has its own
//  context and conceptually acts as a separate process.

#include <vector>
#include <thread>
#include <memory>
#include <functional>

#include <zmq.hpp>
#include "zhelpers.hpp"
#include <chrono>


//  This is our client task class.
//  It connects to the server, and then sends a request once per second
//  It collects responses as they arrive, and it prints them out. We will
//  run several client tasks in parallel, each with a different random ID.
//  Attention! -- this random work well only on linux.

class client_task {
public:
    client_task(const char *id)
        : ctx_(1),
          client_socket_(ctx_, ZMQ_ROUTER),
          my_id_(id)
    {}

    void start() {
        // generate random my_id_
//        char my_id_[10] = {};
//        sprintf(my_id_, "%04X-%04X", within(0x10000), within(0x10000));
        printf("my_id_ %s\n\n", my_id_);
//        client_socket_.setsockopt(ZMQ_IDENTITY, my_id_, strlen(my_id_));
//        client_socket_.connect("tcp://216.165.208.100:5570");
        client_socket_.connect("tcp://localhost:5570");
        zmq::message_t identity(strlen(my_id_));
        memcpy((void *)identity.data(), my_id_, strlen(my_id_));

        int request_nbr = 0;
        int total = 100000;
        try {

            auto start = std::chrono::high_resolution_clock::now();
            while (true) {
//                s_sleep(10);
                char request_string[30] = {};
                sprintf(request_string, "request #%d --%s", ++request_nbr, my_id_);
//                sprintf(request_string, "#%d", ++request_nbr);

                client_socket_.send(identity, ZMQ_SNDMORE);
                client_socket_.send(request_string, strlen(request_string));
                printf("Sent Msg %s\n", request_string);

                zmq::message_t message;
                zmq::message_t id_msg;
                client_socket_.recv(&id_msg);
                client_socket_.recv(&message);
       
                //  Dump the message as text or binary
                int size = message.size();
                std::string data(static_cast<char*>(message.data()), size);
                printf("Received size: %d data: %s\n", size, data.c_str());
              if(request_nbr == total)
                break;
            }

            auto finish = std::chrono::high_resolution_clock::now();
            uint64_t period = std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count();
            int throughput = total * 1000 / period;
            printf("Over! Time: %llu ms Throughtput: %d\n", period, throughput);
        }
        catch (std::exception &e) {}
    }

private:
    zmq::context_t ctx_;
    zmq::socket_t client_socket_;
    const char *my_id_;
};

//  The main thread simply starts several clients and a server, and then
//  waits for the server to finish.

int main (int argc, char *argv[])
{
  std::string id = "loli";
  if (argc > 1)
    id = argv[1];

    client_task ct1(id.c_str());
    ct1.start();

    getchar();
    return 0;
}
