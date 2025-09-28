/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 17:44:19 by ashaheen          #+#    #+#             */
/*   Updated: 2025/09/24 17:56:54 by maram            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_redir_append(t_cmd *cmd, t_token **token_ptr)
{
	char	*filename;

	if (!*token_ptr || !(*token_ptr)->next)
	{
		cmd->redir_error = 1;
		return ;
	}
	*token_ptr = (*token_ptr)->next;
	filename = ft_strdup((*token_ptr)->value);
	if (!filename)
	{
		cmd->redir_error = 1;
		return ;
	}
	open_append_fd(cmd, filename);
	free(filename);
	*token_ptr = (*token_ptr)->next;
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
