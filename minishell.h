/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 14:26:46 by ashaheen          #+#    #+#             */
/*   Updated: 2025/07/26 19:44:59 by ashaheen         ###   ########.fr       */
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
# include <readline/readline.h>
# include <readline/history.h>


typedef enum e_token_type
{
    WORD, // echo
    PIPE, // |
    REDIR_IN, //<
    REDIR_OUT, //>
    REDIR_APPEND, //>>
    HEREDOC //<<
}   t_token_type;


typedef struct s_token
{
    char            *value;     // What is the token? ("echo", ">", "file.txt")
    t_token_type    type;       // What kind? (WORD, REDIR, PIPE, etc.)
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
    struct s_cmd *next;   // pointer to next command (for pipe)
}   t_cmd;


//lexer
void	tokens(char *line, t_token **head);
char *rmv_quotes(const char *s);

//parsing
t_cmd	*parse_pipeline(t_token *token_list);
t_cmd   *parse_cmd(t_token **token_ptr);
void    init_cmd(t_cmd *cmd);
void    handle_redirection(t_cmd *cmd, t_token **token_ptr);

//here_doc
void	    handle_heredoc(t_cmd *cmd, t_token **token_ptr);
void	    add_heredoc(t_heredoc_node **list, char *limiter, int quoted);
t_heredoc   *heredoc_list_to_array(t_heredoc_node *list, int count);
void	    free_heredoc_list(t_heredoc_node *list);

//cmd_args
void	handle_args(t_cmd *cmd, t_token **token_ptr);
void	add_arg(t_arg **list, char *val); // to collect args
char	**arg_list_to_array(t_arg *list); // convert to argv
void	free_arg_list(t_arg *list);       // free if error


//handle signales
void    rl_replace_line(const char *text, int clear_undo);
void    sigint_handler(int sig);
void    setup_signals(void);

//clean
void	free_tokens(t_token *token);
void	*free_arr(char **arr);
void	free_cmd_list(t_cmd *cmd_list);

#endif