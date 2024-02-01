NAME = webserv
CC = c++
CFLAGS = -Wall -Wextra -Werror -g -std=c++98
RM = rm -rf

SRC_DIR = srcs/
SRC_FILES = $(shell find $(SRC_DIR) -type f -name "*.cpp")
OBJ_DIR = objs/
OBJS = $(SRC_FILES:$(SRC_DIR)%.cpp=$(OBJ_DIR)%.o)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJS) 
	@$(CC) $(OBJS) $(LIBFT) $(LDFLAGS) -o $@
	@echo "\r\033[33mDONE!\033[0m         "

clean:
	@$(RM) -r $(OBJ_DIR)

fclean: clean
	@$(RM) $(NAME)

run: re
	@clear
	@echo "\r\033[33mSERVER LAUNCHED!\033[0m         "
	@./$(NAME)

debug: $(NAME)
		lldb ./$(NAME) ConfigFiles/ok.config

re: fclean all

.PHONY: all clean fclean re