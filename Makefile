NAME        = minishell

SRC         = main.c parsing/lexer.c parsing/signal.c parsing/parsing.c \
				parsing/here_doc.c parsing/cmd_args.c parsing/clean.c \
					parsing/lexer_utils.c parsing/redirect.c parsing/expan.c \
						parsing/expan_utils.c execution/execution.c execution/path.c \
							execution/fork.c execution/pipes.c execution/here_doc_ex.c \
								execution/error.c execution/bultin_child.c execution/bultin_cd.c\
								 execution/unset.c execution/exit.c execution/export.c execution/export_utils.c\
								 	execution/export_extend.c
							
OBJ         = $(SRC:.c=.o)

LIBFT_DIR = ./libft
LIBFT := $(LIBFT_DIR)/libft.a
LIBFT_INC := -I$(LIBFT_DIR)

CC          = cc
FLAGS      	= -Wall -Wextra -Werror -I.
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