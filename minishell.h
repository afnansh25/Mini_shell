/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 14:26:46 by ashaheen          #+#    #+#             */
/*   Updated: 2025/10/05 17:28:56 by ashaheen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft/libft.h"
# include <fcntl.h>
# include <limits.h>
# include <stdio.h> 
# include <stdlib.h>
# include <unistd.h>
# include <signal.h>
# include <errno.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <signal.h>

/* one global */
extern int	g_signo;

typedef struct s_shell
{
	int		exit_code;
	char	**envp;
	char	**exp;
}	t_shell;

typedef enum e_token_type
{
	WORD,// echo arg"hello"
	CMD,
	ARG,
	PIPE, // |
	REDIR_IN,// <
	REDIR_OUT, //>
	REDIR_APPEND, //>>
	HEREDOC
}	t_token_type;

typedef enum e_quote_type
{
	NO_QUOTE,
	SINGLE_QUOTE,
	DOUBLE_QUOTE
}	t_quote_type;

typedef struct s_token
{
	char			*value;// What is the token? ("echo", ">", "file.txt")
	t_token_type	type;// What kind? (WORD, REDIR, PIPE, etc.)
	t_quote_type	quote;
	int				ambiguous;
	struct s_token	*next;// Next token in the line (linked list)
}	t_token;

typedef struct s_arg
{
	char			*value;
	struct s_arg	*next;
}	t_arg;

typedef struct s_heredoc //array
{
	char	*limiter;
	int		quoted; //to expand the varible or not
}			t_heredoc;

typedef struct s_heredoc_node
{
	char					*limiter;
	int						quoted; /* to expand the variable or not */
	struct s_heredoc_node	*next;
}	t_heredoc_node;

typedef struct s_cmd
{
	char			**argv;// command + arguments: ["cat", "hello"]
	int				infile;// opened fd or -1 if none
	int				outfile;// opened fd or -1 if none
	int				append;// 1 if >>
	t_heredoc		*heredocs; // Array of heredoc structures
	int				n_heredocs;// Number of heredocs
	struct s_cmd	*next;// pointer to next command (for pipe)
	int				redir_error;
}		t_cmd;

typedef struct s_exec
{
	int		**pipes;
	pid_t	*pids;
	int		cmd_count;
	int		status;
	t_cmd	*cmd_head;
	t_shell	*shell;
}	t_exec;

typedef struct s_export_arg
{
	char	*name;
	char	*value;
	int		has_eq;
	int		append;
}	t_export_arg;

typedef struct s_exp
{
	char		*s;
	char		*res;
	int			i;
	int			f;/* bit0: in_single, bit1: in_double */
	t_shell		*sh;
}	t_exp;

//repl
void			repl_loop(t_shell *sh);
void			process_line(char *line, t_shell *sh);
void			shell_shutdown(t_shell *sh);
char			**dup_envp(char **src);

//parsing part

// lexer
// check
int				check_first_pipe(t_token *t);
int				check_bang(t_token *c);
int				check_double_pipe(t_token *c);
int				check_redir_rules(t_token *c);
int				check_pipe_then_redir(t_token *c);
// err_eyntex
int				err_newline(void);
int				err_pipe(void);
int				err_tok(const char *s);
int				err_event(void);
void			semicolon_error(t_token **head, t_shell *shell, char next);

//handel_quote
int				handle_dollar_quote(const char *s, int *i, char *rs, int *k);
int				handle_plain_quote(const char *s, int *i, char *rs, int *k);
int				handle_backslash(const char *s, int *i, char *rs, int *k);
char			*handle_complex_quotes(const char *s);
//lexer_utils
int				has_unclosed_quote(char *line, t_shell *sh);
t_token			*new_token(char *token, t_token_type type, t_quote_type quote);
void			token_add_back(t_token **token, t_token *new);
void			build_token(char *line, int i, char token[3]);
char			*rmv_quotes(const char *s);
t_quote_type	determine_quote_type(char *raw);
// lexer
t_token_type	get_token_type(char c, char next, int *len);
char			*get_word(char *line, int i, int *len, t_quote_type *quote);
int				create_token(char *line, int *i, t_token **tok, int *len);
int				handle_token(t_token **head, char *line, int *i);
void			tokens(char *line, t_token **head, t_shell *shell);

// quote_utils
int				is_stop(char c);
void			skip_quoted(char *line, int i, int *j, char q);
void			skip_dollar_quote(char *line, int i, int *j);
int				scan_complex_word_length(char *line, int i);

//validate_syntax
int				validate_syntax(t_token *tokens);

//expansion
// expand 

int				do_skip_ctrl(t_exp *x);
char			*expand_variables(char *input, t_shell *shell);
void			expand_token_list(t_token *token, t_shell *shell);

// expand_utils
char			*append_str(char *str, char *suffix);
char			*append_char(char *str, char c);
int				after_redir(t_token *prev, t_token *curr, t_shell *sh);
int				handle_empty_token(t_token **head, t_token **prev,
					t_token **curr, t_shell *sh);
int				remove_empty_tokens(t_token **head, t_shell *sh);

// expand_var
int				do_backslash(t_exp *x);
int				do_singlequote(t_exp *x);
int				do_doublequote(t_exp *x);
int				do_dollar(t_exp *x);
int				do_tilde(t_exp *x);

// var_utils
char			*get_var_value(char *var_name, t_shell *shell);
char			*extract_var_name(char *s, int *len);
char			*handle_dollar(char *input, int *i, t_shell *shell);
char			*handle_tilde(char *input, int *i, t_shell *shell);

//parsing
void			set_token_types(t_token *tokens);
t_cmd			*parse_pipeline(t_token *token_list);
t_cmd			*parse_cmd(t_token **token_ptr);
void			init_cmd(t_cmd *cmd);
int				is_redir(int t);

//here_doc
t_heredoc_node	*collect_heredocs(t_token **token_ptr, int *count);
void			handle_heredoc(t_cmd *cmd, t_token **token_ptr);
int				add_heredoc(t_heredoc_node **list, char *limiter, int quoted);
t_heredoc		*heredoc_list_to_array(t_heredoc_node *list, int count);
void			free_heredoc_list(t_heredoc_node *list);

//redirct
void			handle_redir_append(t_cmd *cmd, t_token **token_ptr);
void			handle_redirection(t_cmd *cmd, t_token **token_ptr);

// redirct_utils
void			errno_msg(const char *s);
int				open_read_fd(t_cmd *cmd, const char *filename);
void			handle_redir_in(t_cmd *cmd, t_token **token_ptr);
void			handle_redir_out(t_cmd *cmd, t_token **token_ptr);
int				open_append_fd(t_cmd *cmd, const char *filename);

//cmd_args
t_arg			*collect_args(t_token **token_ptr);
void			handle_cmd_and_args(t_cmd *cmd, t_token **token_ptr);
int				add_arg(t_arg **list, char *val); // to collect args
char			**arg_list_to_array(t_arg *list); // convert to argv
void			free_arg_list(t_arg *list);// free if error

// cmd_utils
int				copy_argv_to_list(t_arg **list, char **argv);
int				rebuild_argv_from_list(t_cmd *cmd, t_arg *list);
int				add_arg_to_cmd(t_cmd *cmd, char *val);

//handle signales
void			rl_replace_line(const char *text, int clear_undo);
void			sigint_handler(int sig);
void			setup_signals(void);
void			sigint_heredoc_handler(int sig);

//clean
void			free_tokens(t_token *token);
void			*free_arr(char **arr);
void			free_cmd_list(t_cmd *cmd_list);

//----------------------------------EXECUTION----------------
// -------exec-------
//execution
void			execute_pipeline(t_cmd *cmd_list, t_shell *shell);
int				exec_builtin_in_child(t_cmd *cmd, t_shell *shell);
int				exec_builtin_in_parent(t_cmd *cmd, t_shell *shell);
void			wait_all_children(t_exec *exec, t_shell *shell);

//exec_utils
int				count_cmds(t_cmd *cmd);
int				is_child_builtin(char *cmd);
int				is_parent_builtin(char *cmd);
int				is_variable_assignment(char *cmd);
int				handle_variable_assignment(char *assignment, t_shell *shell);

//init_exec
void			init_pipes(t_exec *exec, t_cmd *cmd_list);
void			init_pids(t_exec *exec, t_cmd *cmd_list);
t_exec			*init_exec_struct(t_cmd *cmd_list, t_shell *shell);

// -------fork-------
//fork
void			setup_io(t_cmd *cmd, t_exec *exec, int i);
void			run_child(t_cmd *cmd, t_exec *exec, t_shell *shell, int i);
void			fork_and_execute_all(t_cmd *cmd_list, t_exec *exec,
					t_shell *shell);
//fork_utils
int				ft_isspace(int c);
void			xdup2(int oldfd, int newfd, t_exec *exec);
void			close_if_open(int *fdp);
void			close_pipe_files_child(t_exec *exec);
void			close_pipe_parent(t_exec *exec);
//run_utils
int				is_blank(const char *s);
void			exec_or_error(char *path, t_cmd *cmd, t_exec *exec,
					t_shell *shell);
void			handle_empty_or_blank(t_cmd *cmd, t_exec *exec);
void			handle_var_cmd(t_cmd *cmd, t_exec *exec, t_shell *shell);

//path
char			*get_cmd_path(char *cmd, t_shell *shell, t_exec *exec,
					t_cmd *cmd_list);
char			*resolve_from_path(char *cmd, t_shell *shell, t_exec *exec,
					t_cmd *list);
char			*handle_explicit_path(char *cmd, t_exec *exec, t_cmd *list);
char			*build_cmd_path(char **paths, char *cmd);
char			*find_path_variable(t_shell *shell);

// -------here_doc------------------------------------------------------------
//here_doc_sig
void			hd_set_write_fd(int fd);
void			hd_install_sig(void);
int				hd_should_stop(char *line, char *lim);

//here_doc_ex
void			read_heredoc_input(int write_fd, t_heredoc *hdoc,
					t_shell *shell, t_cmd *cmd_list);
int				handle_here_doc(t_heredoc *hdoc, t_shell *shell,
					t_cmd *cmd_list);
int				handle_all_heredocs(t_cmd *cmd_list, t_shell *shell);
int				process_all_heredocs(t_cmd *cmd, t_cmd *head, t_shell *shell);

//here_doc_utils
char			*expand_line_heredoc(char *line, t_shell *shell);
int				hd_should_stop(char *line, char *lim);
int				hd_wait_and_check(pid_t pid, int rfd, t_shell *shell);

//error
void			error_exit(char *msg, t_exec *exec, t_cmd *cmd_list,
					int exit_code);
void			free_cmd_list(t_cmd *cmd_list);
void			free_exec_data(t_exec *exec);
void			exit_child(t_exec *exec, t_cmd *cmd_list, int exit_code);

//----------------------------------BULTIN--------------------

// -------CHILD-------
//echo
int				exec_echo(char **av);

//pwd		
int				exec_pwd(char **av);

// env		
int				exec_env(char **av, t_shell *shell);
char			*get_env_value(char *name, t_shell *shell);

//env_ut		ils
int				env_count(char **env);
void			free_envp(char **env);
void			init_shlvl(char ***penvp);
// void			init_uid(char ***penvp);

//------PARENT------------
// exit
int				is_numeric_str(char *s);
long long		ft_atoll(const char *s);
unsigned char	normalize_exit_code(long long n);
int				exec_exit(t_cmd *cmd, t_shell *shell, int interactive);
int				is_within_long_long(const char *s);

// unset
int				is_valid_identifier(char *s);
int				env_index_of(char **envp, char *name);
void			env_remove_at(char **envp, int idx);
void			print_unset_invalid(char *name, int *had_error);
int				exec_unset(char **argv, t_shell *shell);

//export
int				parse_export_arg(char *arg, t_export_arg *out);
char			*make_env_pair(char *name, char *value);
int				env_set(char ***penvp, char *name, char *value);
int				env_append(char ***penvp, char *name, char *value);
int				export_one(t_shell *shell, char *arg);

//export_extend
void			print_escaped_value_fd(int fd, const char *s);
int				cmp_env_names(char *a, char *b);
void			sort_env_ptrs(char **a);
char			**join_env_and_exp(char **envp, char **exp);
void			print_decl(char *e);

//export_utils
int				export_index_of(char **exp, char *name);
int				export_add(char ***pexp, char *name);
void			export_remove(char ***pexp, char *name);
void			export_print(char **envp, char **exp);
int				exec_export(char **argv, t_shell *shell);

//export_utils2
int				process_export_token(t_shell *shell, char *arg, int *had_error);
int				env_grow_append(char ***pexp, int n, char *name);
int				set_export_fields(t_export_arg *out, char *arg, int i);
int				export_err_id(char *arg, t_export_arg *o);
char			**append_env(char **envp, char *entry);

//cd
int				exec_cd(char **av, t_shell *shell);
void			set_logical_pwd(char **av, char *target_dir, char *newpwd,
					t_shell *shell);
char			*resolve_target(char **av, t_shell *shell, int *print_newpwd,
					char **alloc);
int				cd_home_case(char **av);
char			*expand_tilde(char *arg, t_shell *shell, char **alloc);
char			*build_entry(char *name, char *value);
int				find_key(char **envp, char *name);
void			update_env_var(char *name, char *value, t_shell *shell);
void			cd_perror(char *path);
char			*dup_env_pwd(t_shell *shell);

#endif