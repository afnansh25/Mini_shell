/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 15:24:28 by ashaheen          #+#    #+#             */
/*   Updated: 2025/07/26 18:00:00 by ashaheen         ###   ########.fr       */
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
}

void    handle_redirection(t_cmd *cmd, t_token **token_ptr)
{
    while (token_ptr)
    {
        if ((*token_ptr)->type == REDIR_IN)
        {
            *token_ptr = (*token_ptr)->next;
            cmd->infile = open((*token_ptr)->value, O_RDONLY);
        }
        if ((*token_ptr)->type == REDIR_OUT)
        {
            *token_ptr = (*token_ptr)->next;
            cmd->outfile = open((*token_ptr)->value, O_CREAT | O_WRONLY | O_TRUNC, 0644);
        }
        if ((*token_ptr)->type == REDIR_APPEND)
        {
            *token_ptr = (*token_ptr)->next;
            cmd->append = cmd->outfile = open((*token_ptr)->value, O_CREAT | O_WRONLY | O_APPEND, 0644);
        }
        if ((*token_ptr)->type == HEREDOC)
            handle_heredoc(cmd, token_ptr);
		else
			*token_ptr = (*token_ptr)->next;
    }
}

t_cmd *parse_cmd(t_token **token_ptr)
{
	t_cmd *cmd;

	printf("ENTERED parse_cmd\n");
	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	init_cmd(cmd);

	while (*token_ptr && (*token_ptr)->type != PIPE)
	{
		printf("🌀 loop: current token = %s (type %d)\n", (*token_ptr)->value, (*token_ptr)->type);

		if ((*token_ptr)->type == REDIR_APPEND || (*token_ptr)->type == REDIR_IN
			|| (*token_ptr)->type == REDIR_OUT || (*token_ptr)->type == HEREDOC)
		{
			handle_redirection(cmd, token_ptr);
			continue; // 🧠 we already advanced *token_ptr inside handle_redirection
		}
		else if ((*token_ptr)->type == WORD)
		{
			handle_args(cmd, token_ptr);
			continue; // 🧠 we already advanced *token_ptr inside handle_args
		}
		else
			*token_ptr = (*token_ptr)->next; // Fallback in case of unexpected token
	}

	printf("✅ parse_cmd: done. First arg: %s\n", cmd->argv ? cmd->argv[0] : "NULL");
	return (cmd);
}


// t_cmd *parse_cmd(t_token **token_ptr)
// {
// 	t_cmd *cmd;

// 	printf("ENTERED parse_cmd\n");
// 	cmd = malloc(sizeof(t_cmd));
// 	if (!cmd)
// 		return (NULL);
// 	init_cmd(cmd);
// 	while (*token_ptr && (*token_ptr)->type != PIPE)
// 	{
// 		if ((*token_ptr)->type == REDIR_APPEND || (*token_ptr)->type == REDIR_IN
// 			|| (*token_ptr)->type == REDIR_OUT || (*token_ptr)->type == HEREDOC)
// 			handle_redirection(cmd, token_ptr);
// 		else if ((*token_ptr)->type == WORD)
// 			handle_args(cmd, token_ptr);
// 		else
// 			*token_ptr = (*token_ptr)->next; 
// 	}
// 	printf("✅ parse_cmd: done. First arg: %s\n", cmd->argv ? cmd->argv[0] : "NULL"); //debug
// 	return (cmd);
// }

t_cmd	*parse_pipeline(t_token *token_list)
{
	t_cmd	*head = NULL;
	t_cmd	*tail = NULL;
	t_cmd	*current;

	printf("🧪 parsed command\n");
	while (token_list)
	{
		current = parse_cmd(&token_list);
		if (!head)
			head = current;
		else
			tail->next = current;
		tail = current;
		if (token_list && token_list->type == PIPE)
			token_list = token_list->next;
	}
	return (head);
}


