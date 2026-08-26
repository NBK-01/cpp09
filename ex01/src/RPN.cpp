#include "../headers/RPN.h"

RPN::RPN() {}

RPN::RPN(const RPN &cpy) : _stack(cpy._stack) {}

RPN &RPN::operator=(const RPN &cpy) {
  if (this != &cpy)
    _stack = cpy._stack;
  return (*this);
}

RPN::~RPN() {}

static double apply(double a, double b, char op) {
  switch (op) {
  case '+':
    return (a + b);
  case '-':
    return (a - b);
  case '*':
    return (a * b);
  case '/':
    if (b == 0)
      throw std::runtime_error(ERR_DIV);
    return (a / b);
  }
  throw std::runtime_error(ERR_BAD);
}

double RPN::evaluate(const str &expr) {
  std::istringstream iss(expr);
  str tok;
  while (iss >> tok) {
    if (tok.size() != 1)
      throw std::runtime_error(ERR_BAD);
    char c = tok[0];
    if (std::isdigit(static_cast<unsigned char>(c))) {
      _stack.push(static_cast<double>(c - '0'));
    } else if (c == '+' || c == '-' || c == '*' || c == '/') {
      if (_stack.size() < 2)
        throw std::runtime_error(ERR_BAD);
      double b = _stack.top();
      _stack.pop();
      double a = _stack.top();
      _stack.pop();
      _stack.push(apply(a, b, c));
    } else {
      throw std::runtime_error(ERR_BAD);
    }
  }
  if (_stack.size() != 1)
    throw std::runtime_error(ERR_BAD);
  return (_stack.top());
}
