NAME        = minishell

SRC         = main.c lexer.c signal.c parsing.c here_doc.c cmd_args.c clean.c

OBJ         = $(SRC:.c=.o)

LIBFT_DIR = ./libft
LIBFT := $(LIBFT_DIR)/libft.a
LIBFT_INC := -I$(LIBFT_DIR)

CC          = cc
FLAGS      	= -Wall -Wextra -Werror
rl_FLAGS    = -I/opt/vagrant/embedded/include/readline -L/opt/vagrant/embedded/lib -lreadline
DEL			= rm -rf

all: $(NAME)

$(NAME): $(LIBFT) $(OBJ)
	$(CC) $(FLAGS) -o $(NAME) $(OBJ) $(rl_FLAGS) $(LIBFT)

%.o: %.c
	$(CC) $(FLAGS) $(LIBFT_INC) -c $< -o $@

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

clean:
	$(DEL) $(OBJ)
	$(MAKE) clean -C $(LIBFT_DIR)

fclean: clean
	$(DEL) $(NAME)
	$(MAKE) fclean -C $(LIBFT_DIR)

re: fclean all

.PHONY: all clean fclean re