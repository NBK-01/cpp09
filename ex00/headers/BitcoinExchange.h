#ifndef BITCOINEXCHANGE_H
#define BITCOINEXCHANGE_H

#include "main.h"

class BitcoinExchange {
private:
  std::map<str, float> _db;

public:
  BitcoinExchange();
  BitcoinExchange(const BitcoinExchange &cpy);
  BitcoinExchange &operator=(const BitcoinExchange &cpy);
  ~BitcoinExchange();

  void loadDB(const str &path);
  void processInput(const str &path);
  float getRate(const str &date) const;
};

#endif
