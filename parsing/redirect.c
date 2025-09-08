/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 17:44:19 by ashaheen          #+#    #+#             */
/*   Updated: 2025/09/05 16:29:32 by ashaheen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void    handle_redir_in(t_cmd *cmd, t_token **token_ptr)
{
    char    *filename;

    if (!(*token_ptr) || !(*token_ptr)->next)
        return ;
    *token_ptr = (*token_ptr)->next;
    filename = ft_strdup((*token_ptr)->value);
    if (!filename)
        return ;
    if (cmd->infile != -1)
        close(cmd->infile);
    cmd->infile = open(filename, O_RDONLY);
    if (cmd->infile == -1)
    {
		ft_putstr_fd("minishell: ", 2);
        perror(filename);
        cmd->redir_error = 1;
    }
    else if (fcntl(cmd->infile, F_SETFD, FD_CLOEXEC) == -1)
    {
        ft_putstr_fd("minishell: ", 2);
        perror("fcntl");
        close(cmd->infile);
        cmd->infile = -1;
        cmd->redir_error = 1;
    }
    // else if (fcntl(cmd->outfile, F_SETFD, FD_CLOEXEC) == -1)
    // {
    //     ft_putstr_fd("minishell: ", 2);
    //     perror("fcntl");
    //     close(cmd->outfile);
    //     cmd->outfile = -1;
    //     cmd->redir_error = 1;
    // }
    free(filename);
    *token_ptr = (*token_ptr)->next;
}

void    handle_redir_out(t_cmd *cmd, t_token **token_ptr)
{
    char    *filename;

    if (!(*token_ptr) || !(*token_ptr)->next)
        return ;
    *token_ptr = (*token_ptr)->next;
    filename = ft_strdup((*token_ptr)->value);
    if (!filename)
        return ;
    if (cmd->outfile != -1)
        close(cmd->outfile);
    cmd->outfile = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (cmd->outfile == -1)
    {
		ft_putstr_fd("minishell: ", 2);
        perror(filename);
        cmd->redir_error = 1;
    }
    free(filename);
    *token_ptr = (*token_ptr)->next;
}

void    handle_redir_append(t_cmd *cmd, t_token **token_ptr)
{
    char    *filename;

    if (!(*token_ptr) || !(*token_ptr)->next)
        return ;
    *token_ptr = (*token_ptr)->next;
    filename = ft_strdup((*token_ptr)->value);
    if (!filename)
        return ;
    if (cmd->outfile != -1)
        close(cmd->outfile);
    cmd->append = 1;
    cmd->outfile = open(filename, O_CREAT | O_WRONLY | O_APPEND, 0644);
    if (cmd->outfile == -1)
    {
		ft_putstr_fd("minishell: ", 2);
        perror(filename);
        cmd->redir_error = 1;
    }
    else if (fcntl(cmd->outfile, F_SETFD, FD_CLOEXEC) == -1)
    {
        ft_putstr_fd("minishell: ", 2);
        perror("fcntl");
        close(cmd->outfile);
        cmd->outfile = -1;
        cmd->redir_error = 1;
    }
    
    free(filename);
    *token_ptr = (*token_ptr)->next;
}

void    handle_redirection(t_cmd *cmd, t_token **token_ptr)
{
    t_token *tok;

    while (*token_ptr && (*token_ptr)->type != PIPE)
    {
        tok = *token_ptr;
        if (tok->type == REDIR_IN)
            handle_redir_in(cmd, token_ptr);
        else if (tok->type == REDIR_OUT)
            handle_redir_out(cmd, token_ptr);
        else if (tok->type == REDIR_APPEND)
            handle_redir_append(cmd, token_ptr);
        else if (tok->type == HEREDOC)
            handle_heredoc(cmd, token_ptr);
        else
            break ;
        if (cmd->redir_error)
            break ;
    }
    if (cmd->redir_error)
        while (*token_ptr && (*token_ptr)->type != PIPE)
            *token_ptr = (*token_ptr)->next;   
}

