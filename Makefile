NAME        = minishell

SRC         = main.c shell_loop.c parsing/lexer/check.c parsing/lexer/err_syntax.c  \
				parsing/lexer/handle_quote.c parsing/lexer/lexer_utils.c parsing/lexer/lexer.c \
				parsing/lexer/quote_utils.c parsing/lexer/validate_syntax.c parsing/clean.c parsing/cmd_args.c parsing/signal.c \
				parsing/here_doc.c parsing/parsing.c parsing/redirect.c parsing/set_token.c  parsing/cmd_utils.c parsing/redirect_utils.c\
				parsing/expansion/expand.c parsing/expansion/expand_utils.c parsing/expansion/expand_var.c \
				parsing/expansion/var_utils.c  parsing/expansion/varr_utils.c \
			  	execution/exec/execution.c execution/exec/exec_utils.c execution/exec/init_exec.c\
			  	execution/fork/fork.c execution/fork/fork_utils.c execution/fork/run_utils.c execution/fork/path.c  \
			  	execution/here_doc/here_doc_ex.c execution/here_doc/here_doc_sig.c execution/here_doc/here_doc_utils.c \
			  	execution/error.c \
			  	bultin/pwd.c bultin/env.c bultin/env_utils.c bultin/echo.c \
              	bultin/cd/cd.c bultin/unset.c  \
				bultin/exit/exit.c bultin/exit/exit_utils.c \
              	bultin/cd/cd_utils.c bultin/export/export.c \
              	bultin/export/export_utils.c bultin/export/export_utils2.c bultin/export/export_extend.c



OBJ         = $(SRC:.c=.o)

LIBFT_DIR   = ./libft
LIBFT       = $(LIBFT_DIR)/libft.a
LIBFT_INC   = -I$(LIBFT_DIR)

# Colors for output
GREEN = \033[0;32m
BLUE = \033[0;34m
PINK = \033[38;5;213m
RESET = \033[0m

CC          = cc
FLAGS       = -Wall -Wextra -Werror -I.
rl_FLAGS    = -I/opt/vagrant/embedded/include/readline -L/opt/vagrant/embedded/lib -lreadline
DEL         = rm -rf

# ASCII Art file
ART_FILE = art/bow.txt

# Hide command output
.SILENT:

all: $(NAME)
	echo ""
	echo "$(PINK)"
	cat $(ART_FILE) || true
	echo "$(RESET)"

$(NAME): $(LIBFT) $(OBJ)
	echo "$(GREEN)Linking minishell...$(RESET)"
	$(CC) $(FLAGS) -o $(NAME) $(OBJ) $(rl_FLAGS) $(LIBFT)

%.o: %.c
	echo "$(BLUE)Compiling $<...$(RESET)"
	$(CC) $(FLAGS) $(LIBFT_INC) -c $< -o $@

$(LIBFT):
	$(MAKE) -s -C $(LIBFT_DIR)

clean:
	echo "$(BLUE)Cleaning object files...$(RESET)"
	$(DEL) $(OBJ)
	$(MAKE) -s clean -C $(LIBFT_DIR)

fclean: clean
	echo "$(BLUE)Cleaning executable...$(RESET)"
	$(DEL) $(NAME)
	$(MAKE) -s fclean -C $(LIBFT_DIR)

re: fclean all

.PHONY: all clean fclean re