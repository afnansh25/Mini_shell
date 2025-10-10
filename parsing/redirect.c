/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maabdulr <maabdulr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 17:44:19 by ashaheen          #+#    #+#             */
/*   Updated: 2025/10/07 16:45:45 by maabdulr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	errno_msg(const char *s)
{
	ft_putstr_fd("minishell: ", 2);
	perror(s);
}

static int	validate_redir_target(t_cmd *cmd, t_token **tok)
{
	if (!*tok || !(*tok)->next)
	{
		cmd->redir_error = 1;
		return (0);
	}
	*tok = (*tok)->next;
	if ((*tok)->ambiguous)
	{
		report_ambiguous_redirect(*tok);
		cmd->redir_error = 1;
		*tok = (*tok)->next;
		return (0);
	}
	return (1);
}

void	handle_redir_append(t_cmd *cmd, t_token **tok)
{
	char	*filename;

	if (!validate_redir_target(cmd, tok))
		return ;
	filename = ft_strdup((*tok)->value);
	if (!filename)
	{
		cmd->redir_error = 1;
		return ;
	}
	if (!open_append_fd(cmd, filename))
	{
		free(filename);
		*tok = (*tok)->next;
		return ;
	}
	free(filename);
	*tok = (*tok)->next;
}

void	handle_redirection(t_cmd *cmd, t_token **token_ptr)
{
	t_token	*tok;

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
	{
		while (*token_ptr && (*token_ptr)->type != PIPE)
			*token_ptr = (*token_ptr)->next;
	}
}

void	report_ambiguous_redirect(t_token *token)
{
	if (!token)
		return ;
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	if (token->value && token->value[0])
	{
		ft_putstr_fd(token->value, STDERR_FILENO);
		ft_putendl_fd(": ambiguous redirect", STDERR_FILENO);
	}
	else
		ft_putendl_fd("ambiguous redirect", STDERR_FILENO);
}
