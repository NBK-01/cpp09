#ifndef RPN_H
#define RPN_H

#include "main.h"

class RPN {
private:
  std::stack<double> _stack;

public:
  RPN();
  RPN(const RPN &cpy);
  RPN &operator=(const RPN &cpy);
  ~RPN();

  double evaluate(const str &expr);
};

#endif
