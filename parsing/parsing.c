/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 15:24:28 by ashaheen          #+#    #+#             */
/*   Updated: 2025/09/24 17:55:39 by maram            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	init_cmd(t_cmd *cmd)
{
	cmd->argv = NULL;
	cmd->infile = -1;
	cmd->outfile = -1;
	cmd->append = 0;
	cmd->heredocs = NULL;
	cmd->n_heredocs = 0;
	cmd->next = NULL;
	cmd->redir_error = 0;
}

int	is_redir(int t)
{
	return (t == REDIR_IN || t == REDIR_OUT
		|| t == REDIR_APPEND || t == HEREDOC);
}

int	process_node(t_cmd *cmd, t_token **cur)
{
	if (is_redir((*cur)->type))
	{
		handle_redirection(cmd, cur);
		if (cmd->redir_error)
			return (2);
		return (1);
	}
	if ((*cur)->type == CMD || (*cur)->type == ARG)
	{
		if (!add_arg_to_cmd(cmd, (*cur)->value))
			return (0);
		*cur = (*cur)->next;
		return (1);
	}
	*cur = (*cur)->next;
	return (1);
}

t_cmd	*parse_cmd(t_token **token_ptr)
{
	t_cmd	*cmd;
	t_token	*cur;
	int		r;

	cmd = malloc(sizeof(*cmd));
	if (!cmd)
		return (NULL);
	init_cmd(cmd);
	handle_cmd_and_args(cmd, token_ptr);
	cur = *token_ptr;
	while (cur && cur->type != PIPE)
	{
		r = process_node(cmd, &cur);
		if (r == 0)
		{
			free_cmd_list(cmd);
			return (NULL);
		}
		if (r == 2)
			break ;
	}
	*token_ptr = cur;
	return (cmd);
}

t_cmd	*parse_pipeline(t_token *token_list)
{
	t_cmd	*head;
	t_cmd	*last;
	t_cmd	*new_cmd;
	t_token	*current;

	head = NULL;
	last = NULL;
	current = token_list;
	while (current)
	{
		new_cmd = parse_cmd(&current);
		if (!head)
			head = new_cmd;
		else
			last->next = new_cmd;
		last = new_cmd;
		if (current && current->type == PIPE)
			current = current->next;
	}
	return (head);
}
