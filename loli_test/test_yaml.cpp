#include "view.hpp"

namespace ppaxos {
  int main(int argc, char** argv) {
    std::string config_file;
    if (argc < 2) {
      std::cout << "Please input the node_id and config file path" << std::endl;
      return 0;
    }

    node_id_t my_id = std::stoul(argv[1]);

    if (argc == 3) {
      config_file = argv[2]; 
    }

    View view(my_id, config_file);
    view.print_host_nodes();
    return 0;
  }
} // namespace ppaxos

int main(int argc, char** argv) {
  return ppaxos::main(argc, argv);
}
