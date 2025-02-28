CC = c++
CFLAGS = -Wall -Wextra -Werror -fsanitize=address -g3 -std=c++98
NAME = webserver

GREEN = \033[1;32m
YELLOW = \033[1;33m
BLUE = \033[1;34m
RED = \033[1;31m
CYAN = \033[1;36m
PURPLE = \033[1;35m
RESET = \033[0m
BOLD = \033[1m

src = $(wildcard src/*.cpp) $(wildcard config/*.cpp)
obj = $(patsubst %.cpp,obj/%.o,$(src))

all: $(NAME)

$(NAME): $(obj)
	@echo "$(GREEN)🔨 Linking $@...$(RESET)"
	@$(CC) $(CFLAGS) -o $@ $^
	@echo "$(BLUE)✨ Compilation complete!$(RESET)"
	@echo "$(PURPLE)🚀 Ready to launch!$(RESET)"

obj/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo "$(YELLOW)📑 Compiling $<...$(RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "$(YELLOW)🧹 Removing object files...$(RESET)"
	@rm -f $(obj)
	@echo "$(RED)✔ Object files removed!$(RESET)"

fclean: clean
	@echo "$(YELLOW)🗑️  Removing executable...$(RESET)"
	@rm -f $(NAME)
	@echo "$(RED)✔ Executable removed!$(RESET)"

re: fclean all

42: all clean
	@echo "$(GREEN)▶️  Running the program...$(RESET)"
	@./$(NAME)

.PHONY: all clean fclean re 42 header