/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 14:26:46 by ashaheen          #+#    #+#             */
/*   Updated: 2025/08/30 13:44:54 by ashaheen         ###   ########.fr       */
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
# include <errno.h>
# include <sys/stat.h>
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
    t_shell  *shell;  //--

}	t_exec;

//parsing part
//lexer
int     handle_token(t_token **head, char *line, int *i);
void    tokens(char *line, t_token **head, t_shell *shell);
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
void    remove_empty_tokens(t_token **head);

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
void	cleanup_on_exit(t_shell *sh, t_cmd *cmd, t_exec *ex);

//execution part
//execution
void execute_pipeline(t_cmd *cmd_list, t_shell *shell);
int exec_builtin_in_ld(t_cmd *cmd, t_shell *shell);
int exec_builtin_in_parent(t_cmd *cmd, t_shell *shell);
int is_parent_builtin(char *cmd);
int is_child_builtin(char *cmd);
int count_cmds(t_cmd *cmd);
void wait_all_children(t_exec *exec, t_shell *shell);
int exec_builtin_in_child(t_cmd *cmd, t_shell *shell);

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
t_exec	*init_exec_struct(t_cmd *cmd_list , t_shell *shell);

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

//----------------------------------BULTIN----------------------------------------------------

// -------CHILD-------
//echo
int exec_echo(char **av);

//pwd
int exec_pwd(char **av);

// env
int exec_env(char **av, t_shell *shell);
char *get_env_value(char *name, t_shell *shell);
int  env_count(char **env);
void    free_envp(char **env);

//------PARENT------------
// exit
int is_numeric_str(char *s);
long long	ft_atoll(const char *s);
unsigned char	normalize_exit_code(long long n);
int	exec_exit(char **argv, t_shell *shell, int interactive);

// unset
int	is_valid_identifier(char *s);
int env_index_of(char **envp, char *name);
void    env_remove_at(char **envp, int idx);
void	print_unset_invalid(char *name, int *had_error);
int	exec_unset(char **argv, t_shell *shell);

//export
int parse_export_arg(char *arg, char **name, char **value, int *has_eq);
char *make_env_pair(char *name, char *value);
int env_set(char ***penvp, char *name, char *value);
int export_one(t_shell *shell, char *arg);
int	exec_export(char **argv, t_shell *shell);
int   export_index_of(char **exp, char *name);
int   export_add(char ***pexp, char *name);
void   export_remove(char ***pexp, char *name);
void    print_escaped_value_fd(int fd, const char *s);
int cmp_env_names(char *a, char *b);
void sort_env_ptrs(char **a);
char	**join_env_and_exp(char **envp, char **exp);
void	export_print(char **envp, char **exp);
//cd
int	exec_cd(char **av, t_shell *shell);
void	set_logical_pwd(char **av, char *target_dir, char *newpwd, t_shell *shell);
char	*resolve_target(char **av, t_shell *shell, int *print_newpwd, char **alloc);
int	cd_home_case(char **av);
char	*expand_tilde(char *arg, t_shell *shell, char **alloc);
char	*build_entry(char *name, char *value);
int	find_key(char **envp, char *name);
char	**append_env(char **envp, char *entry); //0
void	update_env_var(char *name, char *value, t_shell *shell);
void	cd_perror(char *path);


void	print_decl(char *e);
// ------------------------------------------------------------------------------------------------------

#endif