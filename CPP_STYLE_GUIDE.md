# The nkanaan C++ Style Bible

> This document describes **exactly** how I write C++ in the 42 CPP modules (cpp00–cpp09).
> It is written for any AI agent or developer who needs to add code that is indistinguishable
> from mine. Follow it to the letter. When in doubt, imitate the existing files, not "best practice."

The overriding rule: **C++98, strictly.** Everything compiles with `-Wall -Wextra -Werror -std=c++98`.
No `auto`, no range-based `for`, no `nullptr` (use `NULL`), no smart pointers, no lambdas,
no `override`/`final`, no `constexpr`, no initializer lists, no `<cstdint>` fixed-width types.

---

## 1. Directory & File Layout

Every exercise lives in its own self-contained folder and builds its own binary:

```
cppXX/exYY/
├── main.cpp            # the driver / test harness
├── Makefile
├── headers/
│   ├── main.h          # shared includes, typedefs, color macros (per exercise)
│   ├── ClassName.h     # one header per class
│   └── Template.h      # templates live here too, fully implemented inline
└── src/
    └── ClassName.cpp   # one source per class
```

Rules and habits:

- **There is no top-level build.** You `cd cppXX/exYY && make`. Never write a global Makefile.
- Class headers use the **`.h`** extension, not `.hpp`. (`.hpp` shows up only occasionally, e.g.
  `ScalarConverter.hpp`, `Serializer.hpp` — but the default and dominant choice is `.h`.)
- **Templates are implemented inline in the header.** No `.tpp`, no split. See `Array.h`,
  `MutantStack.h`, `easyfind.h`, and the `addRange` template method inside `Span.h`.
- Tiny exercises (cpp00 ex00, cpp01 ex02/ex04, cpp07) may skip the `src/` split and put
  everything in `main.cpp` + `headers/main.h`. That's fine for exercises with no real class.
- Compiled binaries are committed to the repo alongside sources (e.g. `FragTrap`, `ScavTrap`).
  Don't be alarmed by them; they're just leftover build artifacts.

### Include path convention

- From `src/ClassName.cpp`: include with a relative `../headers/` prefix
  → `#include "../headers/ClapTrap.h"`
- From `main.cpp`: include with a `headers/` prefix
  → `#include "headers/FragTrap.h"`
- Between headers in the same folder: bare name → `#include "Contact.h"`, `#include "main.h"`,
  `#include "./ClapTrap.h"`.

---

## 2. The Makefile (signature style)

This is one of the most recognizable parts of my code. Copy it structurally. Template:

```makefile
NAME = FragTrap
AUTHOR = nkanaan
COMP = c++
FLAGS = -Wall -Wextra -Werror -std=c++98

SRC = main.cpp src/ClapTrap.cpp src/ScavTrap.cpp src/FragTrap.cpp
OBJS = $(addprefix objs/, $(SRC:.cpp=.o))

SHELL := /bin/bash

COM_COLOR   = \033[0;34m
OBJ_COLOR   = \033[0;36m
OK_COLOR    = \033[0;32m
ERROR_COLOR = \033[0;31m
WARN_COLOR  = \033[0;33m
NO_COLOR    = \033[m

all: header $(NAME)

header:
	@printf "%b" "$(OK_COLOR)"
	@echo ".------..------."
	@echo "|4.--. ||2.--. |"
	@echo "| :/\: || (\/) |"
	@echo "| :\/: || :\/: |"
	@echo "| '--'4|| '--'2|"
	@echo "'------''------'"
	@echo "NKANAAN but in CPP"
	@printf "%b" "$(OBJ_COLOR)Author:	$(WARN_COLOR)$(AUTHOR)\n"
	@printf "%b" "$(OBJ_COLOR)Compiler: $(WARN_COLOR)$(COMP)\n\033[m"
	@printf "%b" "$(OBJ_COLOR)Flags: 	$(WARN_COLOR)$(FLAGS)\n\033[m"
	@echo

objs/%.o: %.cpp
	@mkdir -p $(dir $@)
	@${COMP} ${FLAGS} -c $< -o $@

$(NAME): $(OBJS)
	@$(COMP) $(FLAGS) -o $(NAME) $(OBJS)
	@printf "$(OK_COLOR)$(NAME) compiled successfully!$(NO_COLOR)\n"

clean:
	@rm -rf objs
	@printf "$(OBJ_COLOR)Objects removed$(NO_COLOR)\n"

re: clean all

fclean: clean
	@rm -f $(NAME)
	@printf "$(OBJ_COLOR)$(NAME) removed$(NO_COLOR)\n"
```

Non-negotiable Makefile habits:

- Variables named `NAME`, `AUTHOR` (always `nkanaan`), `COMP = c++`, `FLAGS` in that exact order.
- `SRC` is an **explicit hand-written list**, never a wildcard.
- Objects go into an **`objs/` directory that mirrors the source tree**; the rule does
  `@mkdir -p $(dir $@)` so `src/Foo.cpp` → `objs/src/Foo.o`.
- **Every recipe line is silenced with `@`** and prints its own colored status message via `printf`.
- A `header` target prints ASCII art + author/compiler/flags, and `all: header $(NAME)`.
- Standard targets: `all`, `header`, `clean`, `fclean`, `re`. `clean` removes `objs`, `fclean` also
  removes the binary. `re: clean all`.
- Both `$(VAR)` and `${VAR}` styles appear — I'm not strict about which within the compile rule.
- `SHELL := /bin/bash` is set (needed for `printf %b` / `echo` escape handling).

> Note: `create_proj.sh` scaffolds a *simpler* Makefile (flat, no `objs/`, no colors). The
> hand-maintained exercises use the richer version above — prefer that one for anything real.

---

## 3. Header Files

### Header guards — belt *and* suspenders

I use a classic include guard **and** a redundant `#pragma once`:

```cpp
#ifndef BUREAUCRAT_H
# define BUREAUCRAT_H

#pragma once

// ...

#endif // !BUREAUCRAT_H
```

Details that recur:
- Note the **space after `#`** on the define line: `# define BUREAUCRAT_H` (a 42/Norminette habit).
  Sometimes `#include` inside guards is also indented as `# include`.
- The closing line is often `#endif // !CLASSNAME_H` (Vim-generated), sometimes just `#endif //`
  with nothing after, sometimes bare `#endif`. All three appear; don't sweat consistency here.
- Guard macro is the filename uppercased with `_H` (e.g. `SCAVTRAP_H`).

### The shared `main.h` / per-header preamble

Most class headers or the exercise's `main.h` open with typedefs and the full ANSI color macro block.
**Reproduce this block verbatim** — it appears in nearly every module:

```cpp
typedef std::string str;
typedef std::exception except;   // added from cpp05 onward, where exceptions appear

#define RESET        "\e[0m"
#define BOLD         "\e[1m"
#define UNDERLINE    "\e[4m"
#define CLEAR		 "\e[H\e[2J"

#define BLACK        "\e[30m"
#define RED          "\e[31m"
#define GREEN        "\e[32m"
#define YELLOW       "\e[33m"
#define BLUE         "\e[34m"
#define MAGENTA      "\e[35m"
#define CYAN         "\e[36m"
#define WHITE        "\e[37m"
```

- `str` is my universal alias for `std::string`; I use `str` far more than `std::string` in
  signatures and locals once it's defined.
- `except` aliases `std::exception`, used as the base for nested exception classes.
- Colors use the `\e[..m` escape form (not `\033[..m`) in **C++** headers, whereas the **Makefile**
  uses `\033[..m`. Keep that split.
- The include set at the top of `main.h` is generous: `<string>`, `<iostream>`, `<ostream>`,
  `<exception>`, plus whatever the exercise needs (`<vector>`, `<algorithm>`, `<limits>`, ...).

### Class declaration layout

I have **two coexisting brace/indent styles**. Match whichever the surrounding module uses;
if starting fresh, the Allman style below is my most common.

**Style A — Allman braces, TAB indent, access labels indented (cpp00–cpp05):**

```cpp
class ClapTrap
{
	protected:
		str _name;
		int	hitPoints;
		int	energyPoints;
		int	attackDamage;
	public:
		ClapTrap(str _name);
		~ClapTrap();
		ClapTrap(const ClapTrap &other);
		ClapTrap &operator=(const ClapTrap &clapTrap);
		void	attack(str const & target);
		void	takeDamage(unsigned int amount);
		void	beRepaired(unsigned int amount);
};
```

- Opening brace on its **own line**.
- `private:` / `protected:` / `public:` are **indented one tab** inside the class.
- Members under a label are indented a **further** tab.
- Return types and names are often **tab-aligned** into columns (`void	attack`, `int	hitPoints`).

**Style B — K&R braces, 4-space indent, access labels flush-left (cpp07/cpp08 templates):**

```cpp
class Span {
private:
    unsigned int _maxSize;
    std::vector<int> _data;

public:
    Span(unsigned int n);
    Span(const Span& other);
    Span& operator=(const Span& other);
    ~Span();

    void addNumber(int number);
    // ...
};
```

- Opening brace on the **same line** as `class Name {`.
- Access labels **flush-left**, 4-space indentation for members.
- This style dominates the STL/template-heavy later modules.

**Ordering inside a class:** members (`private`) sometimes first (Style B) or last (Style A).
Constructors → destructor → copy/assign → getters → methods is the rough intent, but I frequently
declare the default ctor, then group the rest under `/*--- section ---*/` comment dividers.

### Section-divider comments

I annotate declarations with slash-star dividers or short `//` labels:

```cpp
Fixed();
/*------- Constructors ---------*/
Fixed(const int val);
Fixed(const float val);
Fixed(const Fixed &other);
/*------- Destructor ----------*/
~Fixed();
/*------- Operator Overloads ---*/
Fixed& operator=(const Fixed &other);
/*-----------Set & Get------------------*/
void	setRawBits(const int raw);
int		getRawBits() const;
/*--------- Member functions ----------*/
```

or the terse `//`-comment-per-method style:

```cpp
// get name
str getName() const;
// get grade
int getGrade() const;
// increment grade
void increment();
```

### Nested exception classes

Exceptions are declared as **public nested classes** deriving from `except` (my `std::exception`
alias), with the canonical C++98 throw signature:

```cpp
class GradeTooHighException : public except
{
	public:
		virtual const char* what() const throw();
};
```

Implemented in the `.cpp` as:

```cpp
const char *Bureaucrat::GradeTooHighException::what() const throw()
{
	return ("Grade too high!");
}
```

Note `virtual`, `const throw()` (C++98 dynamic exception spec), and the returned string literal
wrapped in parentheses.

### Templates

- Declared with `template <typename T>` — and I often leave a **blank line** between the
  `template` line and the `class`/function it applies to (see `Array.h`, `easyfind.h`).
- Fully implemented inline in the header, with method bodies right in the class body (Style B braces).
- Iterator typedefs use `typedef typename ...`:
  `typedef typename std::stack<T>::container_type::iterator iterator;`

---

## 4. Source Files (`.cpp`) — implementation habits

### Orthodox Canonical Form, always

From cpp02 onward every real class provides the four: default constructor, copy constructor,
copy-assignment operator, destructor. I even leave a teaching comment about it:

```cpp
/*introducing "orthodox canonical form" from now on all classes must contain*/
/*	a constrcutor*/
/*	a copy constructor*/
/*	a copy assignment op*/
/*	a destructor*/
```

(Yes, including the typo `constrcutor` — that's the actual comment. Don't "fix" my voice.)

### Lifecycle logging

Constructors/destructors/operators print a message announcing themselves. This is a defining habit:

```cpp
Fixed::Fixed(): _val(0)
{
	std::cout << GREEN "Default constructor called" RESET << std::endl;
}

Fixed::Fixed(const Fixed &other)
{
	std::cout << YELLOW "Copy constructor called" RESET << std::endl;
	*this = other;
}

Fixed::~Fixed()
{
	std::cout << RED "Destructor called" RESET << std::endl;
}
```

Conventions:
- Default/int/float ctors → `GREEN`, copy/assign → `YELLOW`, destructor → `RED`.
- Message wording: `"<Class> constructor called"`, `"Copy constructor called"`,
  `"Copy assignation operator called"` (I say **"assignation"**, not "assignment", in the printouts),
  `"<Class> destructor called"`.

### Copy constructor delegates to assignment

Canonical pattern — the copy ctor just calls `operator=`:

```cpp
ClapTrap::ClapTrap(const ClapTrap &other)
{
	std::cout << "ClapTrap copy constructor called" << std::endl;
	*this = other;
}
```

Sometimes compressed to one line: `Bureaucrat::Bureaucrat(Bureaucrat const &other) {*this = other;}`

### Assignment operator

Self-assignment guard, member-by-member copy, `return (*this);` **with parentheses**:

```cpp
Fixed& Fixed::operator=(const Fixed &other)
{
	std::cout << YELLOW "Copy assignation operator called" RESET << std::endl;
	if (this != &other)
		this->_val = other._val;
	return (*this);
}
```

For derived classes, chain to the base explicitly:

```cpp
ScavTrap &ScavTrap::operator=(const ScavTrap &scavTrap)
{
	std::cout << "ScavTrap assignation operator called" << std::endl;
	ClapTrap::operator=(scavTrap);
	return (*this);
}
```

### Member-initializer lists

Used for real initialization, all on one line when short:

```cpp
ClapTrap::ClapTrap(std::string _name) : _name(_name), hitPoints(100), energyPoints(50), attackDamage(20)
{ ... }

Span::Span(unsigned int n) : _maxSize(n) {}
```

- Note I happily **shadow** the member name with the parameter name (`ClapTrap(str _name) : _name(_name)`)
  and rely on the init-list to disambiguate. That's my habit; keep it.
- Trivial bodies collapse to `{}` on the same line: `Harl::Harl() {}`, `Span::~Span() {}`.

### `this->` usage

I reach for `this->` inside method bodies fairly often even when unnecessary
(`this->_val = raw;`, `this->energyPoints = this->energyPoints - 1;`) — but not universally.
In getters/one-liners I drop it (`return _name;`). Match the local density.

### Getters

`const` member functions, terse one-liners when trivial:

```cpp
str Bureaucrat::getName() const {return _name;}
int Bureaucrat::getGrade() const {return _grade;}
```

Named `getXxx()`. In the older cpp00 code they're snake_case (`get_size`, `set_item`) — see naming.

---

## 5. Naming Conventions

| Thing | Convention | Examples |
|---|---|---|
| Classes | `PascalCase` | `ClapTrap`, `ScavTrap`, `Bureaucrat`, `ScalarConverter` |
| Member variables | `_camelCase` with leading underscore | `_name`, `_val`, `_maxSize`, `_gradeToSign` |
| Methods (modern, cpp02+) | `camelCase` | `attack`, `takeDamage`, `beRepaired`, `getRawBits`, `addNumber` |
| Methods (early, cpp00) | `snake_case` | `get_size`, `set_item`, `get_items` |
| Free/helper functions | `snake_case` or `fn_` prefix | `fn_add`, `fn_search` |
| Static file-local helpers | `camelCase` | `printChar`, `printFromInt`, `isDisplayable` |
| Constants / macros | `UPPER_SNAKE` | `GRADE_MIN`, `GRADE_MAX`, `RED`, `RESET` |
| Type aliases | short lowercase | `str`, `except` |

Important honesty: **member-variable prefixing is inconsistent.** The `_` prefix is the intent
and the majority, but early classes have unprefixed members (`hitPoints`, `energyPoints`,
`attackDamage`, `size`, `contacts`). If editing an existing class, match *that class*; if writing
new code, use the `_` prefix.

Local variables are short and lowercase, often single letters for scalars: `c`, `d`, `f`, `i`, `l`,
`index`, `endptr`, `fn`, `ln`, `nn`.

---

## 6. Printing & Output — the personality layer

This is the soul of my code. Output is **colorful, decorated, emoji-laden, and funny.**

### Color macro concatenation (no `<<` for colors)

Colors are C-string macros glued directly to the message via **adjacent string-literal
concatenation**, not streamed:

```cpp
std::cout << RED "ClapTrap " << _name << " is already dead!" RESET << std::endl;
std::cout << GREEN BOLD "char: " RESET;
std::cout << BOLD YELLOW "---------- WARNING -----------" << std::endl;
```

So the pattern is `COLOR "literal text"` and `"literal text" RESET`, with real variables streamed
via `<<` in between. Stack multiple attributes: `BOLD RED`, `GREEN BOLD`, `BOLD GREEN`.

### Decorative separators & banners

I frame output with ASCII rules and section headers:

```cpp
std::cout << YELLOW "------------- STATS -------------" << std::endl;
std::cout << "--------------------------------" RESET << std::endl;
std::cout << "---------- WARNING -----------" << std::endl;
std::cout << "<<<-------------- 🗡️🗡️🗡️🗡️🗡️ ------------------>>>>" << std::endl;
```

Emojis are used liberally in flavor lines (🗡️ attack, 🛡️ damage, 🩹 repair). Extra vertical
whitespace is baked right into the literals (`"...!\n\n\n"`) before `RESET`.

### Always `std::endl`

I use `std::endl` (not `"\n"` as the terminator) to end lines, though embedded `\n` inside a
literal is common for blank lines. Errors go to `std::cerr`.

### Voice: informal, self-deprecating, jokey

Messages read like a person talking. Preserve this tone — it's a signature, not noise:

- `"oh well, your phonebook is empty. sad"`
- `"Warning: it's not that good of a phonebook"`
- `"no luck today :()"`
- `"What span? it's just 2 numbers"`
- `"Too much, give me something realistic"`
- `"Do it yourself, it's just 2 numbers"`
- `"I don't know what you're talking about"`
- Harl's mocking sPoNgEbOb case: `"i tHinK yOu sHoUlD uSe cOut iNstEAd Of pRinTf"`

Smiley style is `:()` / `;()` (a quirky variant), and `:(` in comments (`"sorry :("`).

---

## 7. Idioms, Control Flow, Formatting

- **Parenthesized returns:** `return (0);`, `return (*this);`, `return (minSpan);`, `return (i);`.
  Very common, though not 100% (`return *this;` and `return _name;` also appear). Lean toward parens.
- **`return ;` with a space** before the semicolon to bail early from `void` functions. Same with
  `break ;` and `continue ;`.
- **Infinite loops as `for (;;)`**, not `while (true)`.
- **`main` signatures:** `int main()`, `int main(void)`, or `int main (void)` (note the stray space
  variant) — all appear. End with `return (0);` or fall off the end.
- **Casts:** In later modules (cpp06+), always `static_cast<T>(...)` — never C-style casts for real
  conversions. In older code C-style casts appear (`(int)phonebook.get_size()`, `(float)this->_val`).
  New code: prefer `static_cast`.
- **`NULL`, never `nullptr`.** Pointer checks like `char* endptr = NULL;`.
- **Input hardening:** `std::cin.ignore(10000, '\n');`, `std::cin >> std::ws;`, check `std::cin.fail()`
  then `std::cin.clear()`, check `std::cin.eof()` to break. `std::getline(std::cin, x)` for lines.
- **`std::numeric_limits<int>::max()/min()`** for bounds; `strtod`/`strtol` with `endptr` for parsing.
- **Function-pointer dispatch** instead of long if-chains where it's fun to (Harl):
  ```cpp
  void (Harl::*func[])(void) = {&Harl::debug, &Harl::info, &Harl::error, &Harl::warning};
  str  lvls[] = {"DEBUG", "INFO", "ERROR", "WARNING"};
  for (int i = 0; i < 4; i++)
      if (lvls[i] == level) { (this->*func[i])(); return ; }
  ```
- **`try/catch`** around throwing operations; catch `const except &e` (or `const std::exception& e`)
  and print `e.what()` to `std::cerr` in red. Both a compact one-line `catch (...) {...}` and a
  multi-line form appear.
- **Const-correctness:** `const` on getters and on read-only reference params
  (`str const & target`, `Bureaucrat const &executor`). I mix east/west const freely
  (`const str name` and `str const &other` both appear) — don't normalize it.

### Indentation & whitespace, honestly

- **Tabs** are the primary indent in cpp00–cpp06. **4 spaces** in the cpp07/cpp08 template files.
  Match the file you're in.
- Spacing around operators and in declarations is a bit loose: `Array &operator=`, `void	attack`
  (tab-aligned), `str const & target` (spaces around `&`). Don't over-tidy; mirror the neighbors.
- Blank line often left at the very top of a `.cpp` before the first `#include` (a few files start
  with an empty line). Harmless quirk.

---

## 8. C++98 Compliance Checklist (hard requirement)

Everything must build clean under `-Wall -Wextra -Werror -std=c++98`. Before considering code "mine":

- [ ] No `auto`, range-for, `nullptr`, lambdas, `override`/`final`, `constexpr`, initializer lists.
- [ ] No smart pointers; manual `new[]`/`delete[]` with matched cleanup in the destructor
      (see `Array`: `delete[] _array;` in dtor, and in `operator=` before realloc).
- [ ] Exception specs written the C++98 way: `const char* what() const throw();`.
- [ ] `NULL` for null pointers.
- [ ] Iterating containers with explicit iterator types or index counters and
      `std::vector<int>::size_type` (not `size_t` shortcuts where the pedantic type matters).
- [ ] `static_cast` for conversions in new code.
- [ ] Only C++98 headers/STL (`<vector>`, `<stack>`, `<algorithm>`, `<limits>`, `<cmath>`,
      `<cstdlib>`, `<cctype>`, `<iomanip>`, `<iterator>`).

> Watch-out: a couple of headers `#include <regex>`/leftover includes that aren't C++98-clean have
> slipped in historically. Do **not** imitate those — they're mistakes, not the style. When you see
> `<regex>`, treat it as a bug to avoid, not a pattern to follow.

---

## 9. Quick-Start Recipe for a New Class

To add a class the way I would:

1. Create `headers/Foo.h`:
   - Guard `#ifndef FOO_H` / `# define FOO_H` / `#pragma once`.
   - `#include "main.h"` (for `str`, colors, common includes) or the needed standard headers.
   - Declare the class with the four canonical members + your methods, using the surrounding
     module's brace style, with `/*--- section ---*/` dividers.
   - Declare `std::ostream& operator<<(std::ostream&, const Foo&);` as a free function if it prints.
   - Close with `#endif // !FOO_H`.
2. Create `src/Foo.cpp`:
   - `#include "../headers/Foo.h"`.
   - Implement OCF with **colored lifecycle prints** (GREEN ctor, YELLOW copy/assign, RED dtor),
     copy ctor delegating via `*this = other;`, assignment with self-check and `return (*this);`.
   - Getters `const`, one-liners where trivial.
   - Put personality and color into any user-facing output.
3. Add `src/Foo.cpp` to the Makefile `SRC` list (explicitly).
4. In `main.cpp`, `#include "headers/Foo.h"` and write a small, chatty test driver in `int main()`
   ending in `return (0);`.
5. `make` — expect the ASCII banner, then `Foo compiled successfully!`.

---

### TL;DR — the fingerprint

C++98 only • per-exercise folders with `headers/`+`src/` • colored/ASCII-banner Makefile with `objs/`
mirror and `nkanaan` as author • `#ifndef`+`#pragma once` guards • `typedef std::string str;` + the
`\e[..m` color macro block in every header • Orthodox Canonical Form with **colored lifecycle
prints** ("assignation operator called") • copy-ctor delegates to `operator=` • `return (x);` with
parens • `_camelCase` members (mostly), `PascalCase` classes, `camelCase` methods • colors via
adjacent string-literal concatenation (`RED "text" RESET`) • emojis, separators, and a funny,
informal, self-deprecating voice in every message.
