NAME = btc
AUTHOR = nkanaan
COMP = c++
FLAGS = -Wall -Wextra -Werror -std=c++98

SRC = main.cpp src/BitcoinExchange.cpp 
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

ifeq ($(shell test -e ./btc && test -d objs/ && echo -n yes), yes)
	@echo "nothing to be done for: all"
else
	@printf "%b" "$(OBJ_COLOR)Author:	$(WARN_COLOR)$(AUTHOR)\n"
	@printf "%b" "$(OBJ_COLOR)Compiler: $(WARN_COLOR)$(COMP)\n\033[m"
	@printf "%b" "$(OBJ_COLOR)Flags: 	$(WARN_COLOR)$(FLAGS)\n\033[m"
	@echo
endif


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
