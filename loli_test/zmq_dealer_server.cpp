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
    server_worker()
        : ctx_(1),
          worker_(ctx_, ZMQ_ROUTER)
    {}

    void work() {
            worker_.bind("tcp://*:5570");

        try {
            while (true) {
                zmq::message_t identity;
                zmq::message_t msg;
                zmq::message_t copied_id;
                zmq::message_t copied_msg;
//                printf("Waiting for identity\n");
                worker_.recv(&identity);
                int size_id = identity.size();
                std::string data_id(static_cast<char*>(identity.data()), size_id);
                worker_.recv(&msg);
                int size_msg = msg.size();
                std::string data_msg(static_cast<char*>(msg.data()), size_msg);
//                printf("Received %s from %s\n", data_msg.c_str(), data_id.c_str());
//                printf("Received %s\n", data_msg.c_str());

                copied_id.copy(&identity);
                copied_msg.copy(&msg);
                worker_.send(copied_id, ZMQ_SNDMORE);
                worker_.send(copied_msg);

//                mut.lock();
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
//                mut.unlock();
            }
        }
        catch (std::exception &e) {}
    }

private:
    zmq::context_t ctx_;
    zmq::socket_t worker_;
};

//  The main thread simply starts several clients and a server, and then
//  waits for the server to finish.

int main (void)
{
    server_worker * worker = new server_worker();
    worker->work();                                                  

    getchar();
    return 0;
}
