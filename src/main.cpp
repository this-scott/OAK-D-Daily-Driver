#include <iostream>
#include "dai_controller.hpp"

int main(int argc, char* argv[]) {  
  
  std::cout << "Starting webcam driver" << std::endl;
  // open dai camera and create dai stream
  int res = dai_run();
  std::cout << "Camera exit with code " << res << std::endl;

  return res;
}
