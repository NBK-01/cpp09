#include "headers/RPN.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << RED "Error: " ARG_ERR RESET << std::endl;
    return (1);
  }
  try {
    RPN rpn;
    double result = rpn.evaluate(argv[1]);
    std::cout << GREEN << result << RESET << std::endl;
  } catch (const except &e) {
    std::cerr << RED "Error: " RESET << e.what() << std::endl;
    return (1);
  }
  return (0);
}
