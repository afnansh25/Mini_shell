NAME        = minishell

SRC         = main.c parsing/lexer.c parsing/signal.c parsing/parsing.c \
              parsing/here_doc.c parsing/cmd_args.c parsing/clean.c \
              parsing/lexer_utils.c parsing/redirect.c parsing/expan.c \
              parsing/expan_utils.c \
			  execution/exec/execution.c execution/exec/exec_utils.c execution/exec/init_exec.c\
			  execution/fork/fork.c execution/fork/fork_utils.c execution/fork/run_utils.c execution/fork/path.c  \
			  execution/here_doc/here_doc_ex.c execution/here_doc/here_doc_sig.c execution/here_doc/here_doc_utils.c \
			  execution/error.c \
			  bultin/pwd.c bultin/env.c bultin/echo.c \
              bultin/cd/cd.c bultin/unset.c bultin/exit.c \
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
ASAN_FLAGS  = -fsanitize=address -g3 -O0
rl_FLAGS    = -I/opt/vagrant/embedded/include/readline -L/opt/vagrant/embedded/lib -lreadline
DEL         = rm -rf

# ASCII Art file
ART_FILE = art/bow.txt

# Hide command output
.SILENT:
# ---------------------------------------------------------------------

# ===== Homebrew autodetect =====
BREW_PREFIX ?= $(shell brew --prefix 2>/dev/null)
ifeq ($(BREW_PREFIX),)
# Fallbacks (Apple Silicon first, then Intel)
BREW_PREFIX := /opt/homebrew
endif

# Readline (required)
READLINE_ROOT := $(shell brew --prefix readline 2>/dev/null)
ifeq ($(READLINE_ROOT),)
READLINE_ROOT := $(BREW_PREFIX)/opt/readline
endif

# Ncurses (optional; only if present)
NCURSES_ROOT := $(shell brew --prefix ncurses 2>/dev/null)
# If brew didn't return a path, try common fallback
ifeq ($(NCURSES_ROOT),)
NCURSES_ROOT := $(BREW_PREFIX)/opt/ncurses
endif

# ===== Include / Link flags =====
# Headers
CPPFLAGS   += -I. $(LIBFT_INC) -I$(READLINE_ROOT)/include
# Linker paths + libs
LDFLAGS    += -L$(READLINE_ROOT)/lib
LDLIBS     += -lreadline
# Add ncurses only if its lib directory exists (avoids warnings)
ifneq ($(wildcard $(NCURSES_ROOT)/lib),)
LDFLAGS    += -L$(NCURSES_ROOT)/lib
LDLIBS     += -lncurses
endif
# ---------------------------------------------------------------------


all: $(NAME)
	echo ""
	echo "$(PINK)"
	cat $(ART_FILE) || true
	echo "$(RESET)"

$(NAME): $(LIBFT) $(OBJ)
	echo "$(GREEN)Linking minishell...$(RESET)"
		$(CC) $(CFLAGS) $(CPPFLAGS) $(ASAN_FLAGS) -o $(NAME) $(OBJ) $(LIBFT) $(LDFLAGS) $(LDLIBS)

asan: CFLAGS += $(ASAN_FLAGS)
asan: LDFLAGS += $(ASAN_FLAGS)
asan: $(NAME)

%.o: %.c
	echo "$(BLUE)Compiling $<...$(RESET)"
		$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

clean:
	echo "$(BLUE)Cleaning object files...$(RESET)"
	rm -rf $(OBJ)
	$(MAKE) clean -C $(LIBFT_DIR)

fclean: clean
	echo "$(BLUE)Cleaning executable...$(RESET)"
	rm -rf $(NAME)
	$(MAKE) fclean -C $(LIBFT_DIR)

re: fclean all