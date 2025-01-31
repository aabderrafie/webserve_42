CC = c++
CFLAGS =  -Wall -Wextra -Werror -fsanitize=address -g3
NAME = webserver

GREEN = \033[0;32m
YELLOW = \033[0;33m
BLUE = \033[0;34m
RED = \033[0;31m
RESET = \033[0m

src =  $(wildcard src/*.cpp) #$(filter-out src/Request.cpp,
obj = $(src:.cpp=.o)


all: $(NAME)

$(NAME): $(obj)
	@echo "$(GREEN)Linking $@...$(RESET)"
	@$(CC) $(CFLAGS) -o $@ $^
	@echo "$(BLUE)✔ Compilation complete!$(RESET)"

%.o: %.cpp
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "$(YELLOW)Removing object files...$(RESET)"
	@rm -f $(obj)
	@echo "$(RED)✔ Object files removed!$(RESET)"

fclean: clean
	@echo "$(YELLOW)Removing executable...$(RESET)"
	@rm -f $(NAME)
	@echo "$(RED)✔ Executable removed!$(RESET)"

re: fclean all

42: all clean
	@echo "$(GREEN)Running the program...$(RESET)"
	@./$(NAME)

.PHONY: all clean fclean re 42