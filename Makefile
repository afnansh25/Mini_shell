NAME        = minishell

SRC         = main.c lexer.c signal.c

OBJ         = $(SRC:.c=.o)

CC          = cc
FLAGS      	= -Wall -Wextra -Werror
rl_FLAGS   = -I/opt/vagrant/embedded/include/readline -L/opt/vagrant/embedded/lib -lreadline
DEL			= rm -rf

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(FLAGS) -o $(NAME) $(OBJ) $(rl_FLAGS)

%.o: %.c
	$(CC) $(FLAGS) -c $< -o $@

clean:
	$(DEL) $(OBJ)

fclean: clean
	$(DEL) $(NAME)

re: fclean all

.PHONY: all clean fclean re