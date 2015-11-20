//  Asynchronous client-to-server (DEALER to ROUTER)
//
//  While this example runs in a single process, that is to make
//  it easier to start and stop the example. Each task has its own
//  context and conceptually acts as a separate process.

#include <vector>
#include <thread>
#include <memory>
#include <functional>
#include <mutex>

#include <zmq.hpp>
#include "zhelpers.hpp"

//  .split worker task
//  Each worker task works on one request at a time and sends a random number
//  of replies back, with random delays between replies:

uint64_t count = 0;
int total = 100000;
std::chrono::high_resolution_clock::time_point start;
std::mutex mut;

class server_worker {
public:
    server_worker(zmq::context_t &ctx, int sock_type)
        : ctx_(ctx),
          worker_(ctx_, sock_type)
    {}

    void work() {
            worker_.connect("inproc://backend");


        try {
            while (true) {
                zmq::message_t identity;
                zmq::message_t msg;
                zmq::message_t copied_id;
                zmq::message_t copied_msg;
                worker_.recv(&identity);
                int size_id = identity.size();
                std::string data_id(static_cast<char*>(identity.data()), size_id);
                worker_.recv(&msg);
                int size_msg = msg.size();
                std::string data_msg(static_cast<char*>(msg.data()), size_msg);
//                printf("Received %s from %s\n", data_msg.c_str(), data_id.c_str());

                copied_id.copy(&identity);
                copied_msg.copy(&msg);
                worker_.send(copied_id, ZMQ_SNDMORE);
                worker_.send(copied_msg);

                mut.lock();
                count++;
                if (count == 1) {
                  start = std::chrono::high_resolution_clock::now();
                }
                if (count == total) {
                  auto finish = std::chrono::high_resolution_clock::now();
                  uint64_t period = std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count();
                  int throughput = (total - 1) * 1000 / period;
                  printf("Server one time Over! Time: %llu ms Throughtput: %d\n", period, throughput);
                  count = 0;
                }
                mut.unlock();
            }
        }
        catch (std::exception &e) {}
    }

private:
    zmq::context_t &ctx_;
    zmq::socket_t worker_;
};


//  .split server task
//  This is our server task.
//  It uses the multithreaded server model to deal requests out to a pool
//  of workers and route replies back to clients. One worker can handle
//  one request at a time but one client can talk to multiple workers at
//  once.


class server_task {
public:
    server_task()
        : ctx_(1),
          frontend_(ctx_, ZMQ_ROUTER),
          backend_(ctx_, ZMQ_DEALER)
    {
        printf("CONSTRUCT here!\n");
    }

    enum { kMaxThread = 1 };

    void run() {
        printf("Start here!\n");
        frontend_.bind("tcp://*:5570");
        backend_.bind("inproc://backend");


//        printf("I'm here!");
//        server_worker * worker = new server_worker(ctx_, ZMQ_DEALER);
//        worker->work();
        
        std::vector<server_worker *> worker;
        std::vector<std::thread *> worker_thread;
        for (int i = 0; i < kMaxThread; ++i) {
            worker.push_back(new server_worker(ctx_, ZMQ_DEALER));

            worker_thread.push_back(new std::thread(std::bind(&server_worker::work, worker[i])));
            worker_thread[i]->detach();
        }

        printf("Before here!\n");
        try {
            zmq::proxy(frontend_, backend_, nullptr);
        }
        catch (std::exception &e) {}

        for (int i = 0; i < kMaxThread; ++i) {
            delete worker[i];
            delete worker_thread[i];
        }
    }


private:
    zmq::context_t ctx_;
    zmq::socket_t frontend_;
    zmq::socket_t backend_;
};


//  The main thread simply starts several clients and a server, and then
//  waits for the server to finish.

int main (void)
{
        printf("main start\n");
    server_task st;
        printf("Construct finish\n");
    st.run();

    getchar();
    return 0;
}
