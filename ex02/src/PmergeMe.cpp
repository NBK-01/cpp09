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

// (value, id): the id survives the reorders, so partners are recovered without
// a map, and duplicates never collide
typedef std::pair<int, int> Elem;

// comment out to drop the comparison counter
#define COUNT_CMP

#ifdef COUNT_CMP
static long g_cmp = 0;
#endif

static bool byValue(const Elem &a, const Elem &b) {
#ifdef COUNT_CMP
  ++g_cmp;
#endif
  return (a.first < b.first);
}

// b3,b2, b5,b4, b11..b6, ... : Jacobsthal groups walked backwards, 0-based
static std::vector<size_t> jacobOrder(size_t m) {
  std::vector<size_t> order;
  if (m <= 1)
    return (order);
  size_t prevPeak = 1; // b1 is already placed
  size_t jPrev = 1;
  size_t jCurr = 3;
  while (prevPeak < m) {
    size_t peak = (jCurr > m) ? m : jCurr;
    for (size_t p = peak; p >= prevPeak + 1; --p) {
      order.push_back(p - 1);
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

template <typename Cont> static void fordJohnson(Cont &data, size_t idCap) {
  typedef typename Cont::iterator Iter;
  size_t n = data.size();
  if (n < 2)
    return;

  bool hasStraggler = (n % 2 == 1);
  Elem straggler = Elem();
  if (hasStraggler)
    straggler = data[n - 1];

  // 1. pair up; loserOf keeps each big's partner across the recursion
  Cont bigs;
  std::vector<Elem> loserOf(idCap);
  for (size_t i = 0; i + 1 < n; i += 2) {
    Elem a = data[i];
    Elem b = data[i + 1];
    if (byValue(a, b))
      std::swap(a, b);
    bigs.push_back(a);
    loserOf[a.second] = b;
  }

  // 2. sort the bigs
  fordJohnson(bigs, idCap);

  // 3. line the partners up with the sorted bigs, straggler last
  Cont pend;
  for (size_t i = 0; i < bigs.size(); ++i)
    pend.push_back(loserOf[bigs[i].second]);
  if (hasStraggler)
    pend.push_back(straggler);

  // 4. merge-insertion: aPos[j] tracks bigs[j] inside chain, so each pend[j] is
  //    inserted by a search bounded by its partner -- that bound is the whole
  //    point. The straggler has no partner and searches unbounded.
  Cont chain(bigs.begin(), bigs.end());
  std::vector<size_t> aPos(bigs.size());
  for (size_t j = 0; j < bigs.size(); ++j)
    aPos[j] = j;

  // b1 is below the smallest big
  chain.insert(chain.begin(), pend[0]);
  for (size_t j = 0; j < aPos.size(); ++j)
    aPos[j] += 1;

  std::vector<size_t> order = jacobOrder(pend.size());
  for (size_t o = 0; o < order.size(); ++o) {
    size_t j = order[o];
    Iter last = (j < aPos.size()) ? chain.begin() + aPos[j] : chain.end();
    Iter it = std::lower_bound(chain.begin(), last, pend[j], byValue);
    size_t p = static_cast<size_t>(it - chain.begin());
    chain.insert(it, pend[j]);
    for (size_t t = 0; t < aPos.size(); ++t)
      if (aPos[t] >= p)
        aPos[t] += 1;
  }

  data = chain;
}

template <typename Cont> static void printSeq(const str &label, const Cont &c) {
  std::cout << label;
  for (typename Cont::const_iterator it = c.begin(); it != c.end(); ++it)
    std::cout << *it << " ";
  std::cout << std::endl;
}

template <typename Raw, typename Tagged>
static void tag(const Raw &src, Tagged &dst) {
  for (size_t i = 0; i < src.size(); ++i)
    dst.push_back(Elem(src[i], static_cast<int>(i)));
}

template <typename Tagged, typename Raw>
static void untag(const Tagged &src, Raw &dst) {
  dst.clear();
  for (size_t i = 0; i < src.size(); ++i)
    dst.push_back(src[i].first);
}

#ifdef COUNT_CMP
// F(n) = sum of ceil(log2(3k/4)) for k in 1..n, the merge-insertion worst case
static long fjBound(size_t n) {
  long total = 0;
  for (size_t k = 1; k <= n; ++k) {
    long c = 0;
    while ((4L << c) < 3L * static_cast<long>(k))
      ++c;
    total += c;
  }
  return (total);
}
#endif

void PmergeMe::run() {
  printSeq(GREEN "Before: " RESET, _vec);

#ifdef COUNT_CMP
  g_cmp = 0;
#endif
  std::clock_t s1 = std::clock();
  std::vector<Elem> tv;
  tag(_vec, tv);
  fordJohnson(tv, tv.size());
  untag(tv, _vec);
  std::clock_t e1 = std::clock();
#ifdef COUNT_CMP
  long vecCmp = g_cmp;
  g_cmp = 0;
#endif

  std::clock_t s2 = std::clock();
  std::deque<Elem> td;
  tag(_deq, td);
  fordJohnson(td, td.size());
  untag(td, _deq);
  std::clock_t e2 = std::clock();
#ifdef COUNT_CMP
  long deqCmp = g_cmp;
#endif

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
#ifdef COUNT_CMP
  // stderr: stdout must end with the second container's time
  long bound = fjBound(_vec.size());
  std::cerr << MAGENTA "Comparisons: vector " << vecCmp << ", deque " << deqCmp
            << "  |  Ford-Johnson optimum F(" << _vec.size() << ") = " << bound
            << (vecCmp <= bound ? "  OK" : "  OVER") << RESET << std::endl;
#endif
}
