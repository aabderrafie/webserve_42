CC = c++
CFLAGS = -Wall -Wextra -Werror -fsanitize=address -g3
NAME = webserver

# Colors and effects
GREEN = \033[1;32m
YELLOW = \033[1;33m
BLUE = \033[1;34m
RED = \033[1;31m
CYAN = \033[1;36m
PURPLE = \033[1;35m
RESET = \033[0m
BOLD = \033[1m

# Header display function
define HEADER

$(CYAN)
╔══════════════════════════════════════════════════════════════════╗
║                                                                  ║
║     $(PURPLE)██╗    ██╗███████╗██████╗ ███████╗███████╗██████╗ ██╗   ██╗███████╗██████╗ $(CYAN)  ║
║     $(PURPLE)██║    ██║██╔════╝██╔══██╗██╔════╝██╔════╝██╔══██╗██║   ██║██╔════╝██╔══██╗$(CYAN)  ║
║     $(PURPLE)██║ █╗ ██║█████╗  ██████╔╝███████╗█████╗  ██████╔╝██║   ██║█████╗  ██████╔╝$(CYAN)  ║
║     $(PURPLE)██║███╗██║██╔══╝  ██╔══██╗╚════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝██╔══╝  ██╔══██╗$(CYAN)  ║
║     $(PURPLE)╚███╔███╔╝███████╗██████╔╝███████║███████╗██║  ██║ ╚████╔╝ ███████╗██║  ██║$(CYAN)  ║
║     $(PURPLE) ╚══╝╚══╝ ╚══════╝╚═════╝ ╚══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝  ╚══════╝╚═╝  ╚═╝$(CYAN)  ║
║                                                                  ║
║      $(GREEN)Created by: $(YELLOW)aaskal && zbakkas && tlasfar$(CYAN)                           ║
║     $(BLUE)[ Version 42 ]$(CYAN)                                               ║
║                                                                  ║
╚══════════════════════════════════════════════════════════════════╝
$(RESET)
endef
export HEADER

src = $(wildcard src/*.cpp) $(wildcard config/*.cpp) #$(filter-out src/Request.cpp,
obj = $(src:.cpp=.o)

all: header $(NAME)

header:
	@clear
	@echo "$$HEADER"
	@sleep 0.2

$(NAME): $(obj)
	@echo "$(GREEN)🔨 Linking $@...$(RESET)"
	@$(CC) $(CFLAGS) -o $@ $^
	@echo "$(BLUE)✨ Compilation complete!$(RESET)"
	@echo "$(PURPLE)🚀 Ready to launch!$(RESET)"

%.o: %.cpp
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