#ifndef PMERGEME_H
#define PMERGEME_H

#include "main.h"

class PmergeMe {
private:
  std::vector<int> _vec;
  std::deque<int> _deq;

public:
  PmergeMe();
  PmergeMe(const PmergeMe &cpy);
  PmergeMe &operator=(const PmergeMe &cpy);
  ~PmergeMe();

  void parse(int argc, char **argv);
  void run();
};

#endif
