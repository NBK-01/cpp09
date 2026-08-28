#ifndef MAIN_H
#define MAIN_H

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <exception>
#include <iomanip>
#include <iostream>
#include <limits>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

typedef std::string str;
typedef std::exception except;

#define RESET "\e[0m"
#define BOLD "\e[1m"
#define UNDERLINE "\e[4m"
#define CLEAR "\e[H\e[2J"

#define BLACK "\e[30m"
#define RED "\e[31m"
#define GREEN "\e[32m"
#define YELLOW "\e[33m"
#define BLUE "\e[34m"
#define MAGENTA "\e[35m"
#define CYAN "\e[36m"
#define WHITE "\e[37m"

#define ARG_ERR "usage: ./PmergeMe <positive int sequence>  (e.g. 3 5 9 7 4)"
#define ERR_BAD "bad input, feed me positive integers only."

#endif
