#include "../headers/PmergeMe.h"

PmergeMe::PmergeMe() {}

PmergeMe::PmergeMe(const PmergeMe &cpy) : _vec(cpy._vec), _deq(cpy._deq) {}

PmergeMe &PmergeMe::operator=(const PmergeMe &cpy) {
  if (this != &cpy) {
    _vec = cpy._vec;
    _deq = cpy._deq;
  }
  return (*this);
}

PmergeMe::~PmergeMe() {}

/*--------- Parsing ----------*/

void PmergeMe::parse(int argc, char **argv) {
  if (argc < 2)
    throw std::runtime_error(ARG_ERR);
  for (int i = 1; i < argc; ++i) {
    std::istringstream iss(argv[i]);
    str tok;
    while (iss >> tok) {
      for (size_t j = 0; j < tok.size(); ++j)
        if (!std::isdigit(static_cast<unsigned char>(tok[j])))
          throw std::runtime_error(ERR_BAD);
      char *end;
      errno = 0;
      long v = std::strtol(tok.c_str(), &end, 10);
      if (*end != '\0' || v < 0 || v > std::numeric_limits<int>::max())
        throw std::runtime_error(ERR_BAD);
      _vec.push_back(static_cast<int>(v));
      _deq.push_back(static_cast<int>(v));
    }
  }
  if (_vec.empty())
    throw std::runtime_error(ERR_BAD);
}

/*--------- Ford-Johnson (merge-insertion) ----------*/

// order in which the smaller partners get inserted: front element first, then
// the rest grouped by Jacobsthal numbers. pend is 0-indexed (pend[0] == b1 is
// placed separately), so this returns the 0-based order 2,1, 4,3, 10..5, ...
// i.e. the 1-indexed sequence b3,b2, b5,b4, b11..b6, ... shifted to 0-based.
static std::vector<size_t> jacobOrder(size_t m) {
  std::vector<size_t> order;
  if (m <= 1)
    return (order);
  size_t prevPeak = 1;   // last 1-indexed position already placed (b1)
  size_t jPrev = 1;      // J2
  size_t jCurr = 3;      // J3 -> the first Jacobsthal peak
  while (prevPeak < m) {
    size_t peak = (jCurr > m) ? m : jCurr; // clamp to the last real position
    for (size_t p = peak; p >= prevPeak + 1; --p) {
      order.push_back(p - 1); // 1-indexed position -> 0-based pend index
      if (p == prevPeak + 1)
        break;
    }
    if (peak >= m)
      break;
    prevPeak = jCurr;
    size_t nextJ = jCurr + 2 * jPrev; // J(n) = J(n-1) + 2*J(n-2)
    jPrev = jCurr;
    jCurr = nextJ;
  }
  return (order);
}

template <typename Cont> static void fordJohnson(Cont &data) {
  typedef typename Cont::value_type T;
  typedef typename Cont::iterator Iter;
  size_t n = data.size();
  if (n < 2)
    return;

  bool hasStraggler = (n % 2 == 1);
  T straggler = T();
  if (hasStraggler)
    straggler = data[n - 1];

  // 1. pair up: bigs holds the larger of each pair, partners maps big -> smalls.
  //    keying by value keeps every b <= its matched a even with duplicates,
  //    which is all the bound in step 4 needs to stay correct.
  Cont bigs;
  std::map<T, std::vector<T> > partners;
  for (size_t i = 0; i + 1 < n; i += 2) {
    T a = data[i];
    T b = data[i + 1];
    if (a < b)
      std::swap(a, b);
    bigs.push_back(a);
    partners[a].push_back(b);
  }

  // 2. recursively sort the bigs -> the ascending backbone of the main chain
  fordJohnson(bigs);

  // 3. recover each big's smaller partner, aligned to the sorted bigs
  Cont pend;
  for (size_t i = 0; i < bigs.size(); ++i) {
    std::vector<T> &q = partners[bigs[i]];
    pend.push_back(q.back());
    q.pop_back();
  }

  // 4. merge-insertion. chain starts as the sorted bigs; aPos[j] tracks the live
  //    index of bigs[j] inside chain so every b can be inserted with a search
  //    BOUNDED by its own partner's position (that bound is the whole point).
  Cont chain(bigs.begin(), bigs.end());
  std::vector<size_t> aPos(bigs.size());
  for (size_t j = 0; j < bigs.size(); ++j)
    aPos[j] = j;

  // b1 is smaller than every big, so it always lands at the very front
  chain.insert(chain.begin(), pend[0]);
  for (size_t j = 0; j < aPos.size(); ++j)
    aPos[j] += 1;

  std::vector<size_t> order = jacobOrder(pend.size());
  for (size_t o = 0; o < order.size(); ++o) {
    size_t j = order[o];
    Iter last = chain.begin() + aPos[j]; // b[j] <= a[j], so it lands before a[j]
    Iter it = std::lower_bound(chain.begin(), last, pend[j]);
    size_t p = static_cast<size_t>(it - chain.begin());
    chain.insert(it, pend[j]);
    for (size_t t = 0; t < aPos.size(); ++t)
      if (aPos[t] >= p)
        aPos[t] += 1;
  }

  // the straggler has no partner, so it gets a full-range binary search
  if (hasStraggler) {
    Iter it = std::lower_bound(chain.begin(), chain.end(), straggler);
    chain.insert(it, straggler);
  }
  data = chain;
}

/*--------- Output ----------*/

template <typename Cont> static void printSeq(const str &label, const Cont &c) {
  std::cout << label;
  for (typename Cont::const_iterator it = c.begin(); it != c.end(); ++it)
    std::cout << *it << " ";
  std::cout << std::endl;
}

void PmergeMe::run() {
  printSeq(GREEN "Before: " RESET, _vec);

  std::clock_t s1 = std::clock();
  fordJohnson(_vec);
  std::clock_t e1 = std::clock();

  std::clock_t s2 = std::clock();
  fordJohnson(_deq);
  std::clock_t e2 = std::clock();

  printSeq(GREEN "After:  " RESET, _vec);

  double us1 = static_cast<double>(e1 - s1) / CLOCKS_PER_SEC * 1e6;
  double us2 = static_cast<double>(e2 - s2) / CLOCKS_PER_SEC * 1e6;
  std::cout << std::fixed << std::setprecision(5);
  std::cout << YELLOW "Time to process a range of " << _vec.size()
            << " elements with std::vector : " << us1 << " us" RESET
            << std::endl;
  std::cout << YELLOW "Time to process a range of " << _deq.size()
            << " elements with std::deque  : " << us2 << " us" RESET
            << std::endl;
}
