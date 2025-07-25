/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 14:26:46 by ashaheen          #+#    #+#             */
/*   Updated: 2025/07/23 19:24:12 by ashaheen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>


typedef enum e_token_type
{
    WORD,
    PIPE,
    REDIR_IN,
    REDIR_OUT,
    REDIR_APPEND,
    HEREDOC
}   t_token_type;


typedef struct s_token
{
    char            *value;     // What is the token? ("echo", ">", "file.txt")
    t_token_type    type;       // What kind? (WORD, REDIR, PIPE, etc.)
    struct s_token  *next;      // Next token in the line (linked list)
}   t_token;

//lexer
void	tokens(char *line, t_token **head);

//handle signales
void    rl_replace_line(const char *text, int clear_undo); // declare manually
void    sigint_handler(int sig);
void    setup_signals(void);

#endif