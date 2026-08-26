#include "../headers/BitcoinExchange.h"

BitcoinExchange::BitcoinExchange() {}

BitcoinExchange::BitcoinExchange(const BitcoinExchange &cpy) : _db(cpy._db) {} 

BitcoinExchange&  BitcoinExchange::operator=(const BitcoinExchange &cpy)
{
  if (this != &cpy)
    _db = cpy._db;
  return (*this);
}

BitcoinExchange::~BitcoinExchange() {}

void BitcoinExchange::loadDB(const str &path) {
  std::ifstream in(path.c_str());
  if (!in.is_open())
    throw std::runtime_error(str(ERR_OPEN_DB) + path);
  str line;
  // no blind skip of line 1: the header fails isValidDate like any other
  // malformed row, so a headerless csv keeps all of its entries
  while (std::getline(in, line)) {
    line = trim(line);
    if (line.empty())
      continue;
    size_t comma = line.find(',');
    if (comma == str::npos)
      continue;
    str date = trim(line.substr(0, comma));
    float rate;
    if (!isValidDate(date) || !parseRate(trim(line.substr(comma + 1)), rate))
      continue;
    _db[date] = rate;
  }
  if (_db.empty())
    throw std::runtime_error(ERR_EMPTY);
}

float BitcoinExchange::getRate(const str &date) const {
  std::map<str, float>::const_iterator it = _db.lower_bound(date);
  if (it != _db.end() && it->first == date)
    return (it->second);
  if (it == _db.begin()) // no earlier date exists => treat as bad input
    throw std::runtime_error(ERR_BAD);
  --it;
  return (it->second);
}

// the "date | value" header is optional: skip line 1 only when it really is one
static bool isHeaderLine(const str &line) {
  str s;
  for (size_t i = 0; i < line.size(); ++i) {
    unsigned char c = static_cast<unsigned char>(line[i]);
    if (!std::isspace(c))
      s += static_cast<char>(std::tolower(c));
  }
  return (s == "date|value");
}

void BitcoinExchange::processInput(const str &path) {
  std::ifstream in(path.c_str());
  if (!in.is_open())
    throw std::runtime_error(ERR_OPEN);
  str line;
  bool firstLine = true;
  while (std::getline(in, line)) {
    if (firstLine) {
      firstLine = false;
      if (isHeaderLine(line))
        continue;
    }
    if (trim(line).empty())
      continue;
    size_t bar = line.find('|');
    if (bar == str::npos) {
      std::cout << RED "Error: " ERR_BAD RESET << line << std::endl;
      continue;
    }
    str date = trim(line.substr(0, bar));
    float val;
    ValStatus st = parseVal(trim(line.substr(bar + 1)), val);
    if (!isValidDate(date) || st == V_BAD) {
      std::cout << RED "Error: " ERR_BAD RESET << line << std::endl;
      continue;
    }
    if (st == V_NEG) {
      std::cout << RED "Error: " ERR_NEG RESET << std::endl;
      continue;
    }
    if (st == V_BIG) {
      std::cout << RED "Error: " ERR_BIG RESET << std::endl;
      continue;
    }
    try {
      float rate = getRate(date);
      std::cout << GREEN << date << RESET " => " << val << " = " YELLOW
                << val * rate << RESET << std::endl;
    } catch (const except &e) {
      std::cout << RED "Error: " ERR_BAD RESET << line << std::endl;
    }
  }
}

