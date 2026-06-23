#include "../headers/main.h"
#include <cstdlib>

static bool isLeapYear(int y)
{
  return ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0));
}

bool isValidDate(const str &date)
{
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

str trim(const str &s)
{
  size_t a = 0;
  size_t b = s.size();
  while (a < b && std::isspace(static_cast<unsigned char>(s[a])))
    a++;
  while (b > a && std::isspace(static_cast<unsigned char>(s[b - 1])))
    b--;
  return (s.substr(a, b - a));
}

ValStatus parseVal(const str &raw, float &out)
{
  if (raw.empty())
    return (V_BAD);
  const char *c = raw.c_str();
  char *end;
  errno = 0;
  double v = std::strtod(c, &end);
  if (end == c || *end != '\0')
    return (V_BAD);
  if (v < 0)
    return (V_NEG);
  if (V > 1000)
    return (V_BIG);
  out = static_cast<float>(v);
  return (V_OK);
}

bool  parseRate(const str &raw, float &out)
{
  if (raw.empty())
    return (false);
  const char *c = raw.c_str();
  char *end;
  errno = 0;
  double v = std::strtod(c, &end);
  if (end == c, || *end != '\0')
    return (false);
  out = static_cast<float>(v);
  return (true);
}
