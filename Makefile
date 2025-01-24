GREEN = \033[0;32m
YELLOW = \033[0;33m
RED = \033[0;31m
NC = \033[0m 

CC  = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98  -I/usr/include/boost
NAME = webserv
HEADER = $(wildcard *.hpp)
SRCS = $(shell find src -name '*.cpp')
OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	@echo  "$(YELLOW)Linking...$(NC)"
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo  "$(GREEN)Build complete: $(NAME)$(NC)"

%.o: %.cpp $(HEADER)
	@echo  "$(YELLOW)Compiling $<...$(NC)"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo  "$(RED)Cleaning object files...$(NC)"
	@rm -f $(OBJS)

fclean: clean
	@echo  "$(RED)Cleaning all files...$(NC)"
	@rm -f $(NAME)

re: fclean all

42: all clean

.PHONY: all clean fclean re