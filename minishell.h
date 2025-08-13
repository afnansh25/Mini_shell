/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 14:26:46 by ashaheen          #+#    #+#             */
/*   Updated: 2025/08/13 14:52:01 by ashaheen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft/libft.h"
# include <fcntl.h>
# include <stdio.h> 
# include <stdlib.h>
# include <unistd.h>
# include <signal.h>
# include <sys/wait.h>
# include <readline/readline.h>
# include <readline/history.h>

typedef struct s_shell
{
    int exit_code;
    char **envp;
    char **exp;
}	t_shell;

typedef enum e_token_type
{
    WORD, // echo arg"hello"
    CMD,
    ARG,
    PIPE, // |
    REDIR_IN, //<
    REDIR_OUT, //>
    REDIR_APPEND, //>>
    HEREDOC //<<
}   t_token_type;

typedef enum e_quote_type
{
    NO_QUOTE,
    SINGLE_QUOTE,
    DOUBLE_QUOTE
} t_quote_type;


typedef struct s_token
{
    char            *value;     // What is the token? ("echo", ">", "file.txt")
    t_token_type    type;       // What kind? (WORD, REDIR, PIPE, etc.)
    t_quote_type    quote;
    struct s_token  *next;      // Next token in the line (linked list)
}   t_token;

typedef struct s_arg
{
	char *value;
	struct s_arg *next;
}	t_arg;

typedef struct s_heredoc //array
{
    char *limiter;
    int   quoted; //to expand the varible or not
}   t_heredoc;

typedef struct s_heredoc_node //tmp_list
{
    char *limiter;
    int   quoted; //to expand the varible or not
    struct s_heredoc_node *next;
}   t_heredoc_node;

typedef struct s_cmd
{
    char **argv;          // command + arguments: ["cat", "hello"]
    int infile;           // opened fd or -1 if none
    int outfile;          // opened fd or -1 if none
    int append;           // 1 if >>
    t_heredoc   *heredocs; // Array of heredoc structures
    int n_heredocs;       // Number of heredocs
    struct s_cmd *next; // pointer to next command (for pipe)
    int     redir_error;
}   t_cmd;

typedef struct s_exec
{
	int		**pipes;
	pid_t	*pids;
	int		cmd_count;
	int		status;
    t_cmd   *cmd_head;
}	t_exec;

//parsing part
//lexer
int	handle_token(t_token **head, char *line, int *i);
void	tokens(char *line, t_token **head);
char *rmv_quotes(const char *s);
char *get_word(char *line, int i, int *len, t_quote_type *quote);
t_token_type	get_token_type(char c, char next, int *len);

//lexer_utils
int	is_invalid_sequence(char *line, int i);
int scan_word_length(char *line, int i);
void	print_syntax_error(char *line, int i);
t_token	*new_token(char *token, t_token_type type, t_quote_type quote);
void	token_add_back(t_token **token, t_token *new);

//parsing
void set_token_types(t_token *tokens);
t_cmd	*parse_pipeline(t_token *token_list);
t_cmd   *parse_cmd(t_token **token_ptr);
void    init_cmd(t_cmd *cmd);

//here_doc
t_heredoc_node *collect_heredocs(t_token **token_ptr, int *count);
void	    handle_heredoc(t_cmd *cmd, t_token **token_ptr);
int	    add_heredoc(t_heredoc_node **list, char *limiter, int quoted);
t_heredoc   *heredoc_list_to_array(t_heredoc_node *list, int count);
void	    free_heredoc_list(t_heredoc_node *list);

//redirct
void	handle_redirection(t_cmd *cmd, t_token **token_ptr);
void	handle_redir_append(t_cmd *cmd, t_token **token_ptr);
void	handle_redir_out(t_cmd *cmd, t_token **token_ptr);
void	handle_redir_in(t_cmd *cmd, t_token **token_ptr);

//cmd_args
t_arg *collect_args(t_token **token_ptr);
void	handle_cmd_and_args(t_cmd *cmd, t_token **token_ptr);
int 	add_arg(t_arg **list, char *val); // to collect args
char	**arg_list_to_array(t_arg *list); // convert to argv
void	free_arg_list(t_arg *list);       // free if error

//expan
char    *expand_variables(char *input, t_shell *shell);
void    expand_token_list(t_token *token, t_shell *shell);
char    *handle_dollar(char *input, int *i, t_shell *shell);
char    *extract_var_name(char *s, int *len);
char    *get_var_value(char *var_name, t_shell *shell);

//expan_utils
char *append_str(char *str, char *suffix);
char *append_char(char *str, char c);

//handle signales
void    rl_replace_line(const char *text, int clear_undo);
void    sigint_handler(int sig);
void    setup_signals(void);
void	sigint_heredoc_handler(int sig);

//clean
void	print_tokens(t_token *tok); //for debug remove later
void	print_cmd_list(t_cmd *cmd); //debug
void    debug_print_tokens(t_token *tok); //debug
void	free_tokens(t_token *token);
void	*free_arr(char **arr);
void	free_cmd_list(t_cmd *cmd_list);

//execution part
//execution
void execute_pipeline(t_cmd *cmd_list, t_shell *shell);
// int exec_builtin_in_child(t_cmd *cmd);
int exec_builtin_in_parent(t_cmd *cmd, t_shell *shell);
int is_parent_builtin(char *cmd);
int is_child_builtin(char *cmd);
int count_cmds(t_cmd *cmd);
void wait_all_children(t_exec *exec, t_shell *shell);

//heredoc
void handle_all_heredocs(t_cmd *cmd_list, t_shell *shell);
void	process_all_heredocs(t_cmd *cmd, t_shell *shell);
int	handle_here_doc(t_heredoc *hdoc, t_shell *shell, t_cmd *cmd_list);
void	read_heredoc_input(int write_fd, char *limiter, int quoted, t_shell *shell);
char	*expand_line_heredoc(char *line, t_shell *shell);

//path
char	*get_cmd_path(char *cmd, t_shell *shell, t_exec *exec, t_cmd *cmd_list);
char	*build_cmd_path(char **paths, char *cmd);
char	*find_path_variable(t_shell *shell);

//pipes
t_exec	*init_exec_struct(t_cmd *cmd_list);

//fork
void fork_and_execute_all(t_cmd *cmd_list, t_exec *exec, t_shell *shell);
void    run_child(t_cmd *cmd, t_exec *exec, t_shell *shell, int i);
void    close_pipe_parent(t_exec *exec);
void    close_pipe_files_child(t_exec *exec, t_cmd *cmd);
void	setup_io(t_cmd *cmd, t_exec *exec, int i);
void xdup2(int oldfd, int newfd, t_exec *exec);

//error
void error_exit(char *msg, t_exec *exec, t_cmd *cmd_list, int exit_code);
void	free_cmd_list(t_cmd *cmd_list);
void	free_exec_data(t_exec *exec);

//parnet_buitlin
//exit
int         exec_exit(char **argv, t_shell *shell, int interactive);
long long	ft_atoll(const char *s);
int         is_numeric_str(char *s);

//unset
int is_valid_identifier(char *s);
int env_index_of(char **envp, char *name);
int env_index_of(char **envp, char *name);
int exec_unset(char **argv, t_shell *shell);

//export
int parse_export_arg(char *arg, char **name, char **value, int *has_eq);
char *make_env_pair(char *name, char *value);
int env_set(char ***penvp, char *name, char *value);
int export_one(t_shell *shell, char *arg);
int	exec_export(char **argv, t_shell *shell);

void    export_print(char **envp, char **exp);
void	print_escaped_value_fd(int fd, const char *s);
void   export_remove(char ***pexp, char *name);
int   export_add(char ***pexp, char *name);
int   export_index_of(char **exp, char *name);

char	**dup_envp(char **src);
#endif