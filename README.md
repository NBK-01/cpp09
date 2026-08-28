*This project has been created as part of the 42 curriculum by nkanaan.*

# C++ Module 09 — STL

![Standard](https://img.shields.io/badge/standard-C%2B%2B98-blue)
![Flags](https://img.shields.io/badge/flags-Wall%20Wextra%20Werror-orange)
![Module](https://img.shields.io/badge/42-CPP09-black)

---

## Description

The final module of the C++ series, dedicated to the **Standard Template Library**. Three
independent programs each solve a different problem using STL containers, under one
constraint that shapes every design decision: **once a container is used, it can never be
used again for the rest of the module.**

The goal is not to learn container syntax but to learn container *selection* — reading a
problem and recognising which data structure its shape already implies.

| Exercise | Program | Problem | Container |
|---|---|---|---|
| [ex00](#ex00--bitcoin-exchange) | `btc` | Value a bitcoin amount on a given date | `std::map` |
| [ex01](#ex01--reverse-polish-notation) | `RPN` | Evaluate a postfix expression | `std::stack` |
| [ex02](#ex02--pmergeme) | `PmergeMe` | Ford-Johnson merge-insertion sort | `std::vector` + `std::deque` |

All three compile under `c++ -Wall -Wextra -Werror -std=c++98`, warning-free.

---

## Instructions

Each exercise is self-contained with its own Makefile.

```sh
cd ex00 && make        # -> ./btc
cd ex01 && make        # -> ./RPN
cd ex02 && make        # -> ./PmergeMe
```

Available rules: `all`, `clean`, `fclean`, `re`. The Makefiles do not relink.

```
.
├── ex00/
│   ├── data.csv              # exchange rate database (2009-01-02 .. 2022-03-29)
│   ├── headers/              # BitcoinExchange.h, main.h
│   ├── src/                  # BitcoinExchange.cpp, utils.cpp
│   ├── main.cpp
│   └── Makefile
├── ex01/  ...                # RPN.h / RPN.cpp
└── ex02/  ...                # PmergeMe.h / PmergeMe.cpp
```

---

## Usage

### ex00 — Bitcoin Exchange

Reads a file of `date | value` lines and multiplies each value by the bitcoin exchange
rate for that date, taken from `data.csv`.

```sh
./btc input.txt
```

```
2011-01-03 => 3 = 0.9
2011-01-03 => 2 = 0.6
2011-01-03 => 1 = 0.3
Error: not a positive number.
Error: too large a number.
Error: bad input => 2001-42-42
```

Values must be a float or integer between `0` and `1000`. Dates are validated against
real calendar ranges, including leap years. Surrounding whitespace is tolerated and the
`date | value` header line is optional.

### ex01 — Reverse Polish Notation

Evaluates a postfix expression given as a single argument.

```sh
./RPN "8 9 * 9 - 9 - 9 - 4 - 1 +"     # 42
./RPN "7 7 * 7 -"                     # 42
./RPN "1 2 * 2 / 2 * 2 4 - +"         # 0
./RPN "(1 + 1)"                       # Error: bad expression
./RPN "1 0 /"                         # Error: division by zero
```

Single-digit operands (`0`–`9`) and the operators `+ - * /`, as specified by the subject.
Multi-digit numbers, parentheses, stack underflow and leftover operands are all rejected.

### ex02 — PmergeMe

Sorts a positive integer sequence with the Ford-Johnson merge-insertion algorithm, running
it twice over two different containers and timing each.

```sh
./PmergeMe 3 5 9 7 4
./PmergeMe `shuf -i 1-100000 -n 3000 | tr "\n" " "`     # Linux
./PmergeMe `jot -r 3000 1 100000 | tr "\n" " "`         # macOS
```

```
Before: 3 5 9 7 4
After:  3 4 5 7 9
Time to process a range of 5 elements with std::vector : 11.00000 us
Time to process a range of 5 elements with std::deque  : 18.00000 us
```

Accepts positive integers up to `INT_MAX`, given as separate arguments or as one quoted
sequence. Negatives, non-numerics and overflow are rejected. Handles well beyond the
required 3000 elements.

A comparison count is written to **stderr**, so that stdout still ends with the second
container's timing as the subject requires:

```
Comparisons: 7          # for the 5-element example above
```

Comment out `#define COUNT_CMP` in `src/PmergeMe.cpp` to remove it entirely.

---

## Technical choices

### Container selection

- **`std::map`** (ex00) — a sorted tree. Dates in `YYYY-MM-DD` are fixed-width and
  zero-padded, so sorting them as plain strings gives chronological order for free.
  `lower_bound` then answers "closest earlier date" in O(log n), which is precisely the
  rule the subject imposes.
- **`std::stack`** (ex01) — a container adaptor exposing only `push`/`pop`/`top`. The
  restricted interface *is* the RPN discipline: you structurally cannot index into it.
- **`std::vector` + `std::deque`** (ex02) — the two required containers. A vector is one
  contiguous block, fastest at reaching elements; a deque is an array of fixed-size blocks
  and inserts by shifting whichever side is nearer. Merge-insertion places roughly 79% of
  its elements in the lower half of the chain, so the deque moves about a third of the
  data the vector does — which is why it wins at large n, while the vector's faster element
  access wins below roughly 800 elements.

### Ford-Johnson implementation

Elements are compared in pairs; the winners are sorted recursively, then each loser is
inserted into the chain by a binary search bounded by its own partner's position.
Insertions follow **Jacobsthal order** (`b3, b2, b5, b4, b11…b6, …` where
`J(n) = J(n-1) + 2·J(n-2)`), so every search window lands on a power-of-two boundary and
no comparison is wasted.

Each value is tagged as a `(value, id)` pair before sorting. The id survives the
reordering, so partners are recovered without a lookup table and duplicate values never
collide.

The resulting comparison count matches the theoretical optimum
`F(n) = Σ ⌈log₂(3k/4)⌉` — verified for every size from 1 to 120, including already-sorted
and reverse-sorted worst cases, plus n = 3000.

### Known behaviour

A date earlier than `2009-01-02` — the first row in the database — has no lower date to
fall back on, so it is reported as `bad input` rather than clamped upward. Clamping would
mean using the *upper* date, which the subject explicitly warns against.

---

## Conformance

- Compiles clean with `-Wall -Wextra -Werror -std=c++98`
- No `using namespace`, no `friend`, no `*printf`, no `*alloc`/`free`, no `new`
- Every header is independently includable and guarded against double inclusion
- Classes follow the Orthodox Canonical Form
- Errors are written to standard error
- Makefiles provide `$(NAME)`, `all`, `clean`, `fclean`, `re` and do not relink

---

## Resources

### References

- **D. E. Knuth**, *The Art of Computer Programming, Vol. 3: Sorting and Searching* —
  Merge Insertion, §5.3.1 p. 184. The primary source cited by the subject.
- **L. R. Ford Jr. & S. M. Johnson**, ["A Tournament Problem"](https://www.jstor.org/stable/2308750),
  *The American Mathematical Monthly*, Vol. 66 No. 5 (1959) — the original paper.
- **Jacobsthal numbers** — [OEIS A001045](https://oeis.org/A001045).
- **cppreference** — [`std::map`](https://en.cppreference.com/w/cpp/container/map),
  [`std::stack`](https://en.cppreference.com/w/cpp/container/stack),
  [`std::vector`](https://en.cppreference.com/w/cpp/container/vector),
  [`std::deque`](https://en.cppreference.com/w/cpp/container/deque),
  [`std::lower_bound`](https://en.cppreference.com/w/cpp/algorithm/lower_bound).
- **Ford-Johnson lower bound** — comparison counts checked against
  `F(n) = Σ ⌈log₂(3k/4)⌉` and the information-theoretic bound `log₂(n!)`.

### Use of AI

AI assistance was used on this project for the following tasks:

- **Verification and testing** — generating randomised correctness harnesses for ex02
  (sorted-order and multiset checks across n = 1…120 plus n = 3000), cross-checking the
  comparison counts against the Ford-Johnson bound `F(n)`, and instrumented builds used to
  measure the vector/deque insertion behaviour described above.
- **Subject conformance review** — cross-reading the implementation against the subject
  PDF and the module's general rules (forbidden constructs, header independence, output
  stream requirements).
- **Code cleanup** — removing redundant comments and simplifying the comparison-counter
  output.
