/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 18:11:58 by ashaheen          #+#    #+#             */
/*   Updated: 2025/08/10 14:30:07 by maram            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_exec	*init_exec_struct(t_cmd *cmd_list,  t_shell *shell)
{
	t_exec	*exec;
	int		i;

	exec = malloc(sizeof(t_exec));
	if (!exec)
		error_exit("malloc", NULL, cmd_list, 1);
	exec->cmd_head  = cmd_list;
	exec->cmd_count = count_cmds(cmd_list);
	exec->status = 0;
	exec->shell = shell;
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
	exec->pids = malloc(sizeof(pid_t) * exec->cmd_count);
	if (!exec->pids)
		error_exit("malloc", exec, cmd_list, 1);
	return (exec);
}



