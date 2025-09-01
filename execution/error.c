/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 15:41:45 by ashaheen          #+#    #+#             */
/*   Updated: 2025/09/01 17:45:53 by ashaheen         ###   ########.fr       */
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

void exit_child(t_exec *exec, t_cmd *cmd_list, int exit_code)
{
    t_cmd *head;

    head = cmd_list;
    if (!head && exec)
        head = exec->cmd_head;
    if (exec && exec->shell)
        free_envp(exec->shell->envp);
    free_cmd_list(head);
    free_exec_data(exec);
	close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    exit(exit_code);
}

void error_exit(char *msg, t_exec *exec, t_cmd *cmd_list, int exit_code)
{
	ft_putstr_fd("minishell: ", 2);
    if (exit_code == 127)
    {
        ft_putstr_fd(msg, 2);
        if (errno == ENOENT && ft_strchr(msg, '/'))
            ft_putendl_fd(": No such file or directory", 2);
        else
            ft_putendl_fd(": command not found", 2);
    }
	else if (exit_code == 126 && errno == EISDIR)
    {
        ft_putstr_fd(msg, 2);
        ft_putendl_fd(": is a directory", 2);
    }
    else
        {perror(msg);}
	exit_child(exec, cmd_list, exit_code);
}