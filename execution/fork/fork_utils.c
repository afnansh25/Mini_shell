/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fork_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 13:51:20 by maabdulr          #+#    #+#             */
/*   Updated: 2025/09/24 17:08:38 by maram            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_isspace(int c)
{
	return (c == ' ' || c == '\t' || c == '\n'
		|| c == '\v' || c == '\f' || c == '\r');
}

void	xdup2(int oldfd, int newfd, t_exec *exec)
{
	if (dup2(oldfd, newfd) == -1)
		error_exit("dup2", exec, exec->cmd_head, 1);
}

void	close_if_open(int *fdp)
{
	if (*fdp != -1)
	{
		close(*fdp);
		*fdp = -1;
	}
}

void	close_pipe_files_child(t_exec *exec)
{
	int		j;
	t_cmd	*cmd;

	j = 0;
	while (j < exec->cmd_count - 1)
	{
		close_if_open(&exec->pipes[j][0]);
		close_if_open(&exec->pipes[j][1]);
		j++;
	}
	cmd = exec->cmd_head;
	while (cmd)
	{
		close_if_open(&cmd->infile);
		close_if_open(&cmd->outfile);
		cmd = cmd->next;
	}
}

void	close_pipe_parent(t_exec *exec)
{
	int	i;

	i = 0;
	while (i < exec->cmd_count - 1)
	{
		close_if_open(&exec->pipes[i][0]);
		close_if_open(&exec->pipes[i][1]);
		i++;
	}
}
