//
//  Hello World server in C++
//  Binds REP socket to tcp://*:5555
//  Expects "Hello" from client, replies with "World"
//
#include <zmq.hpp>
//#include <string>
//#include <iostream>
//#ifndef _WIN32
//#include <unistd.h>
//#else
//#include <windows.h>
//
//#define sleep(n)	Sleep(n)
//#endif
//
//int main () {
//    //  Prepare our context and socket
//    zmq::context_t context (1);
//    zmq::socket_t socket (context, ZMQ_REP);
//    socket.bind ("tcp://*:5555");
//
//    while (true) {
//        zmq::message_t request;
//
//        //  Wait for next request from client
//        socket.recv (&request);
//        std::cout << "Received Hello" << std::endl;
//
//        //  Do some 'work'
//    	sleep(1);
//
//        //  Send reply back to client
//        zmq::message_t reply (5);
//        memcpy (reply.data (), "World", 5);
//        socket.send (reply);
//    }
//    return 0;
//}

#include <string>
#include <iostream>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#endif

#include "message.pb.h"
#include <google/protobuf/text_format.h>


int main () {
    //  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t socket(context, ZMQ_PAIR);
    socket.bind ("tcp://*:5555");

    while (true) {
        zmq::message_t request;
        //  Wait for next request from client
        socket.recv (&request);
        std::cout << "Received" << std::endl;
        tutorial::Person person;
        std::string msg_str(static_cast<char*>(request.data()), request.size());
        person.ParseFromString(msg_str);
        std::string text_str;
        google::protobuf::TextFormat::PrintToString(person, &text_str);
        std::cout << text_str << std::endl;
    }
    return 0;
}
