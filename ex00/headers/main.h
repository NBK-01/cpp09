#ifndef MAIN_H
#define MAIN_H

#pragma once

#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <sstream>
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

#define ARG_ERR                                                                \
  "no arguments given: format must be ./btc [input.txt] (optional: "           \
  "[data.csv])"

#define ERR_OPEN "could not open file."
#define ERR_BAD "bad input => "
#define ERR_NEG "not a positive number."
#define ERR_BIG "too large a number."
#define ERR_EMPTY "database is empty."

enum ValStatus { V_OK, V_NEG, V_BIG, V_BAD };

bool  isValidDate(const str &date);
str trim(const str &s);
ValStatus parseVal(const str &raw, float &out);
bool  parseRate(const str &raw, float &out);

#endif // !MAIN_H
