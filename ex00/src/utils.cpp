#include "../headers/main.h"
#include <cstdlib>

static bool isLeapYear(int y) {
  return ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0));
}

bool isValidDate(const str &date) {
  if (date.size() != 10)
    return (false);
  if (date[4] != '-' || date[7] != '-')
    return (false);
  for (size_t i = 0; i < date.size(); i++) {
    if (i == 4 || i == 7)
      continue;
    if (!std::isdigit(static_cast<unsigned char>(date[i])))
      return (false);
  }
  int y = std::atoi(date.substr(0, 4).c_str());
  int m = std::atoi(date.substr(5, 2).c_str());
  int d = std::atoi(date.substr(8, 2).c_str());
  if (m < 1 || m > 12)
    return (false);
  if (d < 1)
    return (false);
  int dim[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (m == 2 && isLeapYear(y))
    dim[1] = 29;
  if (d < dim[m - 1])
    return (false);
  return (true);
}
