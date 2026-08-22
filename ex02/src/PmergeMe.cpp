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

// Each element is carried as (value, id). The id is unique and stable across
// every recursion level, so a plain std::vector scratch table keyed by id can
// recover each big's smaller partner after the recursion reorders the bigs.
// That replaces the old std::map (ex00's container, forbidden here) while
// staying duplicate-safe: equal values never collide because ids are unique.
typedef std::pair<int, int> Elem; // (value, id)

static bool byValue(const Elem &a, const Elem &b) { return (a.first < b.first); }

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

// Cont is a container of Elem (std::vector<Elem> or std::deque<Elem>).
// idCap is the number of distinct ids (== original element count), used to size
// the id-keyed recovery table.
template <typename Cont> static void fordJohnson(Cont &data, size_t idCap) {
  typedef typename Cont::iterator Iter;
  size_t n = data.size();
  if (n < 2)
    return;

  bool hasStraggler = (n % 2 == 1);
  Elem straggler = Elem();
  if (hasStraggler)
    straggler = data[n - 1];

  // 1. pair up: bigs holds the larger of each pair, and loserOf[bigId]
  //    remembers each big's partner so it survives the reorder the recursion
  //    below performs (map-free, keyed by the unique id).
  Cont bigs;
  std::vector<Elem> loserOf(idCap);
  for (size_t i = 0; i + 1 < n; i += 2) {
    Elem a = data[i];
    Elem b = data[i + 1];
    if (a.first < b.first)
      std::swap(a, b);
    bigs.push_back(a);
    loserOf[a.second] = b;
  }

  // 2. recursively sort the bigs -> the ascending backbone of the main chain
  fordJohnson(bigs, idCap);

  // 3. recover each big's smaller partner, aligned to the sorted bigs, then
  //    append the straggler (if any) as pend's last, partner-less element so
  //    it joins the very same Jacobsthal insertion order as everyone else
  //    instead of being tacked on afterwards with an extra full-range search.
  Cont pend;
  for (size_t i = 0; i < bigs.size(); ++i)
    pend.push_back(loserOf[bigs[i].second]);
  if (hasStraggler)
    pend.push_back(straggler);

  // 4. merge-insertion. chain starts as the sorted bigs; aPos[j] tracks the live
  //    index of bigs[j] inside chain so every pend[j] that has a partner can be
  //    inserted with a search BOUNDED by that partner's position (that bound is
  //    the whole point). pend's last slot may be the straggler: it has no entry
  //    in aPos (j == aPos.size() once it comes up), so its search runs
  //    unbounded, all the way to chain.end(), instead.
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
    // b[j] <= a[j], so it lands before a[j]; the straggler has no a[j] at all.
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

/*--------- Output ----------*/

template <typename Cont> static void printSeq(const str &label, const Cont &c) {
  std::cout << label;
  for (typename Cont::const_iterator it = c.begin(); it != c.end(); ++it)
    std::cout << *it << " ";
  std::cout << std::endl;
}

// tag raw values with unique ids so the sort can recover partners map-free
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

void PmergeMe::run() {
  printSeq(GREEN "Before: " RESET, _vec);

  // Each clock covers the full data management + sorting part for its
  // container: tagging the raw values, running Ford-Johnson, then untagging
  // back, all inside the same timed window.
  std::clock_t s1 = std::clock();
  std::vector<Elem> tv;
  tag(_vec, tv);
  fordJohnson(tv, tv.size());
  untag(tv, _vec);
  std::clock_t e1 = std::clock();

  std::clock_t s2 = std::clock();
  std::deque<Elem> td;
  tag(_deq, td);
  fordJohnson(td, td.size());
  untag(td, _deq);
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
