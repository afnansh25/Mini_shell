/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maabdulr <maabdulr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 15:24:28 by ashaheen          #+#    #+#             */
/*   Updated: 2025/08/09 18:58:04 by maabdulr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void    init_cmd(t_cmd *cmd)
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
void set_token_types(t_token *tokens)
{
    t_token *current;
    int is_cmd;

    current = tokens;
    is_cmd = 1;
    while (current)
    {
        if (current->type == PIPE)
            is_cmd = 1;
        else if (current->type == WORD)
        {
            if (is_cmd)
            {
                current->type = CMD;
                is_cmd = 0;
            }
            else
                current->type = ARG;
        }
        current = current->next;
    }
}

t_cmd *parse_cmd(t_token **token_ptr)
{
	t_cmd *cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	init_cmd(cmd);
	handle_cmd_and_args(cmd, token_ptr);
	while (*token_ptr && (*token_ptr)->type != PIPE)
	{
		if ((*token_ptr)->type == REDIR_APPEND || (*token_ptr)->type == REDIR_IN
			|| (*token_ptr)->type == REDIR_OUT || (*token_ptr)->type == HEREDOC)
			handle_redirection(cmd, token_ptr);
		else
			*token_ptr = (*token_ptr)->next; // Fallback in case of unexpected token
	}
	return (cmd);
}

t_cmd *parse_pipeline(t_token *token_list)
{
    t_cmd *head;
    t_cmd *last;
    t_cmd *new_cmd;
    t_token *current;

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
    return head;
}




