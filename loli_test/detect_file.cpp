#include "detect_file.hpp"

namespace ppaxos {

int main(int argc, char** argv) {
  int total = 30;
  if (argc > 1) 
    total = std::stoi(argv[1]);

  DetectionFile de(total); 

  de.detect_all();
  de.print_all();

  return 0;
}

}

int main(int argc, char** argv) {
  return ppaxos::main(argc, argv);
}
