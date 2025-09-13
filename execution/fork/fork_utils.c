/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fork_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 13:51:20 by maabdulr          #+#    #+#             */
/*   Updated: 2025/09/13 14:34:12 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int ft_isspace(int c)
{
    return (c == ' '  || c == '\t' || c == '\n' || 
            c == '\v' || c == '\f' || c == '\r');
}

void xdup2(int oldfd, int newfd, t_exec *exec)
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
	int	j;

	j = 0;
	while (j < exec->cmd_count - 1)
	{
		close_if_open(&exec->pipes[j][0]);
		close_if_open(&exec->pipes[j][1]);
		j++;
	}
	while (exec->cmd_head)
	{
		close_if_open(&exec->cmd_head->infile);
		close_if_open(&exec->cmd_head->outfile);
		exec->cmd_head = exec->cmd_head->next;
	}
}

void    close_pipe_parent(t_exec *exec)
{
    int i;

    i = 0;
	while (i < exec->cmd_count - 1)
	{
		close_if_open(&exec->pipes[i][0]);
		close_if_open(&exec->pipes[i][1]);
		i++;
	}
}
