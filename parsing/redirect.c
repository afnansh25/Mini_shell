/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 17:44:19 by ashaheen          #+#    #+#             */
/*   Updated: 2025/10/05 17:30:35 by ashaheen         ###   ########.fr       */
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
	if ((*token_ptr)->ambiguous)
	{
		cmd->redir_error = 1;
		*token_ptr = (*token_ptr)->next;
		return ;
	}
	filename = ft_strdup((*token_ptr)->value);
	if (!filename)
	{
		cmd->redir_error = 1;
		return ;
	}
	if (!open_append_fd(cmd, filename))
	{
		free(filename);
		*token_ptr = (*token_ptr)->next;
		return ;
	}
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
