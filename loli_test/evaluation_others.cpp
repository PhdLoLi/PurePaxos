/**
 * process_single.cpp 
 * Author: Lijing Wang
 * Date: 7/02/2015
 */

#include "view.hpp"
#include "captain.hpp"
#include "detect_file.hpp"
#include <fstream>
#include <boost/filesystem.hpp>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include "zmq_dealer_client.hpp"

namespace ppaxos {

using namespace std;
using namespace boost::filesystem;

std::string my_name;

ofstream my_file;

int total = 10;

void do_sth(slot_id_t slot_id, std::string& data) {
//  LOG_INFO("HAHA slot_id:%llu value:%s", slot_id, data.c_str());
//  my_file << data << "\n";
}

static void sig_int(int num) {
  std::cout << "Control + C triggered! " << std::endl;
//  my_file.close();
//  DetectionFile de(total);
  exit(num);
}  

int main(int argc, char** argv) {
  signal(SIGINT, sig_int);
 

  std::string config_file;

  if (argc < 2) {
    std::cout << "Usage: Node_ID Node_Num" << std::endl;
    return 0;
  }

  node_id_t my_id = std::stoul(argv[1]);
  int node_num = stoi(argv[2]);

  config_file = "config/nodes-" + to_string(node_num) + ".yaml";

  // init view for one captain
  View view(my_id, config_file);
  view.print_host_nodes();
  
  my_name = view.hostname();

  std::string file_name = "tmp/" + my_name + ".txt";
  LOG_INFO("file_name %s", file_name.c_str());

  my_file.open(file_name, ios::out);
  my_file.close();

//  my_file.open(file_name, ios::out | ios::app);

  // init callback
  callback_t callback = do_sth;

  // init one specific captain

  Captain captain(view, callback);

  client_task ct(my_name.c_str());
  ct.reply_ready();

  ct.listen();

  LOG_INFO("I'm all set finish");

  return 0;
}

} // namespace ppaxos

int main(int argc, char** argv) {
  return ppaxos::main(argc, argv);
}
