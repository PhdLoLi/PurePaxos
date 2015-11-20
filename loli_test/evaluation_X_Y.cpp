/**
 * evaluation_X_Y.cpp 
 * Author: Lijing Wang
 * Date: 8/02/2015
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
#include "zmq_dealer_client.hpp"
#include <algorithm>

namespace ppaxos {

using namespace std;
using namespace boost::filesystem;

std::string my_name;
node_id_t my_id;

ofstream my_file;
Captain *captain;
std::string value;
client_task *ct;

int total = 70000;
slot_id_t commit_counter = 0;
int global_counter = 0;
int self_counter = 0;
int send_counter = 0;
int control_sig = 0;
int exe_counter = 0;
boost::mutex counter_mut;
boost::mutex exe_counter_mut;

std::vector<uint64_t> periods;
std::vector<uint64_t> exe_periods;
std::vector<int> trytimes;
std::vector<std::chrono::high_resolution_clock::time_point> starts(total);

std::chrono::high_resolution_clock::time_point start;

void generate_res() {
}

void send_msg() {
  
  send_counter++;
  start = std::chrono::high_resolution_clock::now();
  sleep(5);

  counter_mut.lock();
  exe_counter_mut.lock();

  auto finish = std::chrono::high_resolution_clock::now();
  uint64_t period = std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count();
  int throughput = self_counter * 1000 / period;
  LOG_INFO("self_counter: %d throughput: %d send_counter: %d", self_counter, throughput, send_counter);
  
  std::sort(periods.begin(), periods.end());
  std::sort(exe_periods.begin(), exe_periods.end());
  std::sort(trytimes.begin(), trytimes.end());

  std::vector<uint64_t> sample_periods(3, 0);
  std::vector<uint64_t> sample_exe_periods(3, 0);
  std::vector<int> sample_trytimes(3, 0);

  std::vector<float> percents = {0.5, 0.9, 0.99};
  std::vector<int> percent_counters(3,0); 

  MsgResult *msg_result = new MsgResult();
  msg_result->set_throughput(throughput);
  std::vector<MsgPeriodTryTime *> msg_ptts(3, NULL); 

  for(int i = 0; i < 3; i++) {
    percent_counters[i] = percents[i] * self_counter;
    sample_periods[i] = periods[percent_counters[i]];
    sample_exe_periods[i] = exe_periods[percent_counters[i]];
    sample_trytimes[i] = trytimes[percent_counters[i]];

    msg_ptts[i] = msg_result->add_msg_ptt(); 
    msg_ptts[i]->set_percent(percents[i] * 100);
    msg_ptts[i]->set_period(sample_periods[i]);
    msg_ptts[i]->set_exe_period(sample_exe_periods[i]);
    msg_ptts[i]->set_try_time(sample_trytimes[i]);

    LOG_INFO("percent: %f, period: %llu, exe_period: %llu, trytime: %d", percents[i], sample_periods[i], sample_exe_periods[i], sample_trytimes[i]);
  }

  std::string msg_str;
  
  msg_result->SerializeToString(&msg_str);
    
  // reset all stuff
  self_counter = 0;
  exe_counter = 0;
  periods.clear();
  exe_periods.clear();
  trytimes.clear();

  exe_counter_mut.unlock();
  counter_mut.unlock();



  ct->send_string(msg_str);

  LOG_INFO("send_msg done!");

  if (control_sig != -1)
    send_msg();
}

void do_sth(slot_id_t slot_id, std::string& data) {
  global_counter++;
}

void count_exe_latency(slot_id_t slot_id, PropValue& prop_value, node_id_t node_id) {

  exe_counter_mut.lock();

//  LOG_INFO("HAHA slot_id:%llu value_id:%llu value:%s", slot_id, prop_value.id(), prop_value.data().c_str());

  if (control_sig == 1 && node_id == my_id) {
    value_id_t value_id = prop_value.id() >> 16;
    auto finish = std::chrono::high_resolution_clock::now();
    exe_periods.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(finish-starts[value_id % total]).count());
//    LOG_INFO("exe_periods[%d] = %llu ns value_id : %llu", exe_counter, exe_periods[exe_counter], value_id);

    exe_counter++;
  }  

  exe_counter_mut.unlock();
}

void count_latency(slot_id_t slot_id, PropValue& prop_value, int try_time) {

  if (control_sig == -1) 
    return;

  counter_mut.lock();
  commit_counter++;

  value_id_t value_id = prop_value.id() >> 16;
  if (control_sig == 1) {
    
    auto finish = std::chrono::high_resolution_clock::now();

    periods.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(finish-starts[value_id % total]).count());
    trytimes.push_back(try_time);

//    LOG_INFO("periods[%d] = %llu ns trytime = %d value_id : %llu", self_counter, periods[self_counter], trytimes[self_counter], value_id);

    self_counter++;
  }

  starts[commit_counter % total] = std::chrono::high_resolution_clock::now();
  counter_mut.unlock();

  captain->commit(value);
}

static void sig_int(int num) {
  std::cout << "Control + C triggered! " << std::endl;
  auto finish = std::chrono::high_resolution_clock::now();
  uint64_t period = std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count();
  int throughput = (global_counter - 1) * 1000 / period;
  LOG_INFO("Last_commit -- counter:%d milliseconds:%llu throughput:%d", global_counter, period, throughput);

  exit(num);
}  

int main(int argc, char** argv) {
  signal(SIGINT, sig_int);

  std::string config_file;
  if (argc < 5) {
    std::cout << "Usage: Node_ID Node_Num Value_Size Window_Size" << std::endl;
    return 0;
  }

  my_id = stoul(argv[1]); 
  int node_num = stoi(argv[2]);
  int value_size = stoi(argv[3]);
  int win_size = stoi(argv[4]);

  config_file = "config/nodes-" + to_string(node_num) + ".yaml";

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

  callback_latency_t call_latency = count_latency;

  callback_full_t callback_full = count_exe_latency;

  // init one specific captain

//  Captain captain(view, &tp, callback);
  captain = new Captain(view, win_size);

  captain->set_callback(call_latency);
//  captain->set_callback(callback);
  captain->set_callback(callback_full);

  std::string s_value(512, 'X');
  value = s_value;

  for (int i = 0; i < value_size - 1; i++) {
    value += s_value; 
  }

  // send ACK to controller!
  
  ct = new client_task(my_name.c_str());
  ct->reply_ready();

  // waiting for the instruction  

  for (int i = 0; i < win_size * 2; i++) {
    LOG_INFO("** commit %s", value.c_str());
    counter_mut.lock();
    commit_counter++;
    starts[commit_counter] = std::chrono::high_resolution_clock::now(); 
    counter_mut.unlock();
    captain->commit(value);

//    LOG_INFO("***********************************************************************");
  }

  ct->listen_collect();
  // start sending message


  control_sig = 1;

  boost::thread sending(send_msg);

  ct->listen();
  control_sig = -1;

  sending.join();
  LOG_INFO("I'm all set finish");

  return 0;
}

} // namespace ppaxos

int main(int argc, char** argv) {
  return ppaxos::main(argc, argv);
}
