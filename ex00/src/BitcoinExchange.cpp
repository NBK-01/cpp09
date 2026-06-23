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


