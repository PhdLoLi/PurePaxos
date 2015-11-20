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
#include <chrono>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>


namespace ppaxos {

using namespace std;
using namespace boost::filesystem;

std::string my_name;

ofstream my_file;

int total = 100000;
int counter = 0;
std::chrono::high_resolution_clock::time_point start;

void do_sth(slot_id_t slot_id, std::string& data) {
  counter++;
  if (slot_id == total) {
    auto finish = std::chrono::high_resolution_clock::now();
    uint64_t period = std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count();
    int throughput = (total - 1) * 1000 / period;
    LOG_INFO("Last_commit_value -- slot_id:%llu value:%s milliseconds:%llu throughput:%d", slot_id, data.c_str(), period, throughput);
  } else if (slot_id == 1) {
    start = std::chrono::high_resolution_clock::now();
  } else if (slot_id % 5000 == 0) {
    LOG_INFO("slot_id:%llu value:%s", slot_id, data.c_str());
  }
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
    std::cout << "Please input the node_id and config file path" << std::endl;
    return 0;
  }

  // remove all the files inside tmp/
//  path d("tmp");
//  if (exists(d))
//  {
//    remove_all(d);
//  }
//
//  create_directory(d);

  node_id_t my_id = std::stoul(argv[1]);


  config_file = "config/nodes-5.yaml";

  if (argc == 3) {
    config_file = argv[2]; 
  }

  // init view for one captain
  View view(my_id, config_file);
  view.print_host_nodes();
  
  my_name = view.hostname();

  std::string file_name = "tmp/" + my_name + ".txt";
  LOG_INFO("file_name %s", file_name.c_str());

//  my_file.open(file_name, ios::out);
//  my_file.close();
//
//  my_file.open(file_name, ios::out | ios::app);

  // init thread_pool 
  // pool tp(1);

  // init callback
  callback_t callback = do_sth;

  // init one specific captain

//  Captain captain(view, &tp, callback);
  Captain captain(view, callback);

  for (int i = 0; i < total; i++) {
    std::string value = "Commiting Value Time_" + std::to_string(i) + " from " + view.hostname();
//    LOG_INFO("** commit %s", value.c_str());
    captain.commit_value(value);
//    LOG_INFO("***********************************************************************");
  }

  LOG_INFO("Sleeping for 10000");
  sleep(10000);
  // my_file.close();
//  DetectionFile de(total);

  return 0;
}

} // namespace ppaxos

int main(int argc, char** argv) {
  return ppaxos::main(argc, argv);
}
