#include "headers/PmergeMe.h"

int main(int argc, char **argv) {
  try {
    PmergeMe sorter;
    sorter.parse(argc, argv);
    sorter.run();
  } catch (const except &e) {
    std::cerr << RED "Error: " RESET << e.what() << std::endl;
    return (1);
  }
  return (0);
}
