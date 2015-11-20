//
//  Hello World client in C++
//  Connects REQ socket to tcp://localhost:5555
//  Sends "Hello" to server, expects "World" back
//
#include <zmq.hpp>
//#include <string>
//#include <iostream>
//
//int main ()
//{
//    //  Prepare our context and socket
//    zmq::context_t context (1);
//    zmq::socket_t socket (context, ZMQ_REQ);
//
//    std::cout << "Connecting to hello world server..." << std::endl;
//    socket.connect ("tcp://localhost:5555");
//
//    //  Do 10 requests, waiting each time for a response
//    for (int request_nbr = 0; request_nbr != 10; request_nbr++) {
//        zmq::message_t request (5);
//        memcpy (request.data (), "Hello", 5);
//        std::cout << "Sending Hello " << request_nbr << "..." << std::endl;
//        socket.send (request);
//
//        //  Get the reply.
//        zmq::message_t reply;
//        socket.recv (&reply);
//        std::cout << "Received World " << request_nbr << std::endl;
//    }
//    return 0;
//}

#include "message.pb.h"
#include <string>
#include <iostream>


int main ()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    tutorial::Person person; //fill a person data
    person.set_id(1234);
    person.set_name("john");
    person.set_email("john@mycompany.com");
    tutorial::Person::PhoneNumber* phone_number = person.add_phone();
    phone_number->set_number("1234567");
    phone_number->set_type(tutorial::Person::MOBILE);
    phone_number = person.add_phone();
    phone_number->set_number("7654321");
    phone_number->set_type(tutorial::Person::HOME);

    //  Prepare our context and socket
    zmq::context_t context (1);
    // Note we use here a PAIR socket, only 1 way message
    zmq::socket_t socket (context, ZMQ_PAIR);

    std::cout << "Connecting to server" << std::endl;
    socket.connect ("tcp://localhost:5555");

    std::string msg_str;
    person.SerializeToString(&msg_str);
    // create a zmq message from the serialized string
    zmq::message_t request (msg_str.size());
    memcpy ((void *) request.data (), msg_str.c_str(), msg_str.size());
    std::cout << "Sending Person data ..." << std::endl;
    socket.send (request);

    // Optional:  Delete all global objects allocated by libprotobuf.
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
