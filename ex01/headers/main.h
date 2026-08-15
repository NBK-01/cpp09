#ifndef MAIN_H
#define MAIN_H

#pragma once

#include <cctype>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <ostream>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>

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

#define ARG_ERR "usage: ./RPN \"<expression>\"  (e.g. \"8 9 * 9 - 9 - 9 - 4 - 1 +\")"
#define ERR_BAD "bad expression, do it yourself."
#define ERR_DIV "division by zero, nice try."

#endif // !MAIN_H
