/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fork.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 15:30:31 by ashaheen          #+#    #+#             */
/*   Updated: 2025/08/05 15:47:30 by ashaheen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void xdup2(int oldfd, int newfd, t_exec *exec)
{
	if (dup2(oldfd, newfd) == -1)
		error_exit("dup2", exec, exec->cmd_head, 1);
}

void	setup_io(t_cmd *cmd, t_exec *exec, int i)
{
	if (cmd->infile != -1)
		xdup2(cmd->infile, STDIN_FILENO, exec);
	else if (i > 0)
		xdup2(exec->pipes[i - 1][0], STDIN_FILENO, exec); // read from previous pipe 
	if (cmd->outfile != -1)
		xdup2(cmd->outfile, STDOUT_FILENO, exec);
	else if (i < exec->cmd_count - 1)
		xdup2(exec->pipes[i][1], STDOUT_FILENO, exec); // write to next pipe 
}

void    close_pipe_files_child(t_exec *exec, t_cmd *cmd)
{
    int j;

    j = 0;
    while (j < exec->cmd_count - 1)
    {
        close(exec->pipes[j][0]);
        close(exec->pipes[j][1]);
        j++;
    }
    if(cmd->infile != -1)
    {
        close(cmd->infile);
        cmd->infile = -1;
    }
    if(cmd->outfile != -1)
    {
        close(cmd->outfile);
        cmd->outfile = -1;
    }
}

void    close_pipe_parent(t_exec *exec)
{
    int i;

    i = 0;
    while (i < exec->cmd_count -1)
    {
        if (exec->pipes[i][0] != -1) 
        { 
            close(exec->pipes[i][0]); 
            exec->pipes[i][0] = -1; 
        }
        if (exec->pipes[i][1] != -1) 
        { 
            close(exec->pipes[i][1]); 
            exec->pipes[i][1] = -1; 
        }
        i++;
    }
}

void    run_child(t_cmd *cmd, t_exec *exec, t_shell *shell, int i)
{
    char    *path;

    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    setup_io(cmd, exec, i);
    close_pipe_files_child(exec, cmd);
    if(!cmd->argv || !cmd->argv[0] || cmd->argv[0][0] == '\0')
        exit(0);
    if(is_child_builtin(cmd->argv[0]))
        exit(exec_builtin_in_child(cmd));
    path = get_cmd_path(cmd->argv[0], shell, exec, exec->cmd_head);
    execve(path, cmd->argv, shell->envp);
    perror(cmd->argv[0]); 
    free(path);
    exit(126);
}

void fork_and_execute_all(t_cmd *cmd_list, t_exec *exec, t_shell *shell)
{
    int i;
    t_cmd *cmd;

    i = 0;
    cmd = cmd_list;
    while (cmd)
    {
        exec->pids[i] = fork();
        if (exec->pids[i] < 0)
            error_exit("fork", exec, exec->cmd_head, 1);
        if (exec->pids[i] == 0)
            run_child(cmd, exec, shell, i);
        cmd = cmd->next;
        i++;
    }
    close_pipe_parent(exec);
}
