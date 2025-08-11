/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maabdulr <maabdulr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 15:41:45 by ashaheen          #+#    #+#             */
/*   Updated: 2025/08/09 14:51:01 by maabdulr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_exec_data(t_exec *exec)
{
	int	i;

	if (!exec)
		return ;
	if (exec->pipes)
	{
		i = 0;
		while (i < exec->cmd_count - 1)
		{
			if (exec->pipes[i])
			{
				if (exec->pipes[i][0] != -1)
					close(exec->pipes[i][0]);
				if (exec->pipes[i][1] != -1)
					close(exec->pipes[i][1]);
				free(exec->pipes[i]);
			}
			i++;
		}
		free(exec->pipes);
	}
	if (exec->pids)
		free(exec->pids);
	free(exec);
}

void	free_cmd_list(t_cmd *cmd_list)
{
	t_cmd	*tmp;
	int		i;

	while (cmd_list)
	{
		tmp = cmd_list->next;
		if (cmd_list->argv)
		{
			i = 0;
			while (cmd_list->argv[i])
			{
				free(cmd_list->argv[i]);
				i++;
			}
			free(cmd_list->argv);
		}
		if (cmd_list->heredocs)
		{
			i = 0;
			while (i < cmd_list->n_heredocs)
			{
				free(cmd_list->heredocs[i].limiter);
				i++;
			}
			free(cmd_list->heredocs);
		}
		if (cmd_list->infile != -1)
			close(cmd_list->infile);
		if (cmd_list->outfile != -1)
			close(cmd_list->outfile);
		free(cmd_list);
		cmd_list = tmp;
	}
}

void error_exit(char *msg, t_exec *exec, t_cmd *cmd_list, int exit_code)
{
	free_exec_data(exec);
	free_cmd_list(cmd_list);
	perror(msg);
	exit(exit_code);
}



