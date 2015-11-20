/**
 * test_captain.cpp
 * To test Proposer Acceptor Captain Commo(No Message exchange) 
 * Author: Lijing Wang
 */

#include "commo.hpp"
#include <iostream>
#include <thread>
#include <cstdlib>
#include <fstream>
#include <chrono>
#include "detect_error.hpp"
#include <assert.h>

namespace ppaxos {

void do_sth(slot_id_t slot_id, std::string& data) {
//  LOG_INFO("HAHA slot_id:%llu value:%s", slot_id, data.c_str());
}

// client to commit value
void client_commit_file(Captain * captain) {
  std::string line;
  node_id_t node_id = captain->get_node_id();
  std::string filename = "values/client_" +  std::to_string(node_id);
//  std::cout << "FileName " << filename << std::endl;
  std::ifstream value_file (filename);
  if (value_file.is_open())
  {
    while (getline(value_file, line)) {
//      std::cout << "** client_commit: before commit Node " << node_id << " Value " << line << std::endl;
      LOG_DEBUG("** Before call <captain->client_commit>  --NodeID %u (value):%s", node_id, line.c_str());
      captain->commit_value(line); 
      LOG_DEBUG("** After  call <captain->client_commit>  --NodeID %u (value):%s", node_id, line.c_str());
    }
//    if (line == "") 
//      value_file << "Hello World From Node" + std::to_string(node_id); 
    value_file.close();
    LOG_INFO("** Close File  --NodeID %u", node_id);
  }
  else 
    std::cout << "** Unable to open file" << std::endl; 
}

int main(int argc, char** argv) {
  auto t1 = std::chrono::high_resolution_clock::now();
  LOG_INFO("** START **");

  int node_nums = 5;
  uint64_t total_times = 1;

  if (argc == 1) {
    LOG_INFO("Use default node_nums:%s5%s total_times:%s1%s", TXT_RED, NRM, TXT_RED, NRM);
  }

  if (argc > 1)
    node_nums = atoi(argv[1]);
  if (argc > 2) 
    total_times = atoll(argv[2]);


  std::set<node_id_t> nodes;
  // init all nodes set
  for (int i = 0; i < node_nums; i++) 
    nodes.insert(i);

  std::vector<Captain *> captains;
  std::vector<View *> views;
  // init all view & captain
  for (int i = 0; i < node_nums; i++) {
    views.push_back(new View(i, nodes));
    captains.push_back(new Captain(*views[i]));
  }
  
  Commo commo(captains);
  pool tp(1);
  commo.set_pool(&tp);
  callback_t callback = do_sth;
  // set commo for every captain & init a new client thread
  for (int i = 0; i < node_nums; i++) {
    captains[i]->set_commo(&commo);
    captains[i]->set_callback(callback);
  }

  std::vector<int> node_times_vec(node_nums, 0); 

  int node_id = 0;
  for (int i = 0; i < total_times; i++) {
    auto t2 = std::chrono::high_resolution_clock::now();
    srand(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());
    node_id = rand() % node_nums;
//    std::cout << "node_id " << node_id << std::endl;

    LOG_DEBUG("***********************************************************************");
    LOG_INFO("** This (time):%d (node_id):%d (node_times):%d", i, node_id, node_times_vec[node_id]);
    std::string value = "Love MS Time_" + std::to_string(node_times_vec[node_id]) + " from Node_" + std::to_string(node_id) + " Total_Time_" + std::to_string(i);
    LOG_DEBUG("** Commit Value--[%s] Start", value.c_str());
    captains[node_id]->commit_value(value);
    LOG_DEBUG("** (Client):%d (Commit_Times):%d END", node_id, node_times_vec[node_id]);
    LOG_DEBUG("***********************************************************************");

    node_times_vec[node_id]++;
  }

  tp.wait();
  Detection det(captains, total_times);
  if (!det.detect_all()) 
    det.print_one();
  
  LOG_INFO("** END **");
  return EXIT_SUCCESS;
}
} // namespace ppaxos

int main(int argc, char** argv) {
  return ppaxos::main(argc, argv);
}
