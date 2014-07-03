#include <iostream>
#include <string>

#include "artm/cpp_interface.h"
#include "artm/messages.pb.h"

int main(int argc, char * argv[]) {
  if (argc != 2) {
    std::cout << "Usage:\n\t./node_controller <endpoint>\n\n";
    std::cout << "Example:\n\t./node_controller tcp://*:5555\n\n";
    std::cout << "To connect to node_controller from master replace '*' with\n";
    std::cout << "fully qualified DNS name of the host.\n\n";
    return 0;
  }

  std::cout << "Starting NodeController at " << argv[1] << "... ";
  ::artm::NodeControllerConfig node_config;
  node_config.set_create_endpoint(argv[1]);
  ::artm::NodeController node_controller(node_config);
  std::cout << "OK.\n";

  std::cout << "Type Ctrl+Z or Ctrl+C to stop NodeController and quit.\n\n";
  while(true) {
    std::string str;
    std::cin >> str;
  }

  return 0;
}
