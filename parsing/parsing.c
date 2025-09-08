/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 15:24:28 by ashaheen          #+#    #+#             */
/*   Updated: 2025/09/08 18:16:44 by ashaheen         ###   ########.fr       */
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
    int     is_cmd;

    current = tokens;
    while (current)
    {
        if (current->type == CMD || current->type == ARG)
            current->type = WORD;
        current = current->next;
    }
    current = tokens;
    is_cmd = 1;
    while (current)
    {
        if (current->type == PIPE)
        {
            is_cmd = 1;
            current = current->next;
        }
        else if (current->type == REDIR_IN || current->type == REDIR_OUT
            || current->type == REDIR_APPEND || current->type == HEREDOC)
        {
            current = current->next;
            if (current)
                current = current->next;
        }
        else if (current->type == WORD)
        {
            if (is_cmd)
            {
                current->type = CMD;
                is_cmd = 0;
            }
            else
                current->type = ARG;
            current = current->next;
        }
        else
            current = current->next;
    }
}

t_cmd *parse_cmd(t_token **token_ptr)
{
	t_cmd *cmd;
	t_token *current;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	init_cmd(cmd);
	
	// First pass: collect command and initial arguments
	handle_cmd_and_args(cmd, token_ptr);
	
	// Second pass: handle redirections and collect any remaining arguments
	current = *token_ptr;
    while (current && current->type != PIPE)
    {
        if (current->type == REDIR_APPEND || current->type == REDIR_IN
                || current->type == REDIR_OUT || current->type == HEREDOC)
        {
            handle_redirection(cmd, &current);
            if (cmd->redir_error)
                break;
        }
        else if (current->type == CMD || current->type == ARG)
        {
            if (!add_arg_to_cmd(cmd, current->value))
            {
                free_cmd_list(cmd);
                return (NULL);
            }
            current = current->next;
        }
        else
            current = current->next;
    }
	*token_ptr = current;
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




