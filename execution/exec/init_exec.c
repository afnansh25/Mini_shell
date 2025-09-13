/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_exec.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 18:11:58 by ashaheen          #+#    #+#             */
/*   Updated: 2025/09/13 13:57:40 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	init_pipes(t_exec *exec, t_cmd *cmd_list)
{
	int	i;

	if (exec->cmd_count <= 1)
	{
		exec->pipes = NULL;
		return ;
	}
	exec->pipes = malloc(sizeof(int *) * (exec->cmd_count - 1));
	if (!exec->pipes)
		error_exit("malloc", exec, cmd_list, 1);
	i = 0;
	while (i < exec->cmd_count - 1)
	{
		exec->pipes[i] = malloc(sizeof(int) * 2);
		if (!exec->pipes[i])
			error_exit("malloc", exec, cmd_list, 1);
		if (pipe(exec->pipes[i]) == -1)
			error_exit("pipe", exec, cmd_list, 1);
		i++;
	}
}

void	init_pids(t_exec *exec, t_cmd *cmd_list)
{
	exec->pids = malloc(sizeof(pid_t) * exec->cmd_count);
	if (!exec->pids)
		error_exit("malloc", exec, cmd_list, 1);
}

t_exec	*init_exec_struct(t_cmd *cmd_list, t_shell *shell)
{
	t_exec	*exec;

	exec = malloc(sizeof(t_exec));
	if (!exec)
		error_exit("malloc", NULL, cmd_list, 1);
	exec->cmd_head = cmd_list;
	exec->cmd_count = count_cmds(cmd_list);
	exec->status = 0;
	exec->shell = shell;
	init_pipes(exec, cmd_list);
	init_pids(exec, cmd_list);
	return (exec);
}



