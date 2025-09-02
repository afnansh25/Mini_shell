/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fork.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 15:30:31 by ashaheen          #+#    #+#             */
/*   Updated: 2025/09/02 17:39:18 by ashaheen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <errno.h>
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
        if (exec->pipes[j][0] != -1)
        {
            close(exec->pipes[j][0]);
            exec->pipes[j][0] = -1;
        }
        if (exec->pipes[j][1] != -1)
        {
            close(exec->pipes[j][1]);
            exec->pipes[j][1] = -1;
        }
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

int ft_isspace(int c)
{
    return (c == ' '  || c == '\t' || c == '\n' || 
            c == '\v' || c == '\f' || c == '\r');
}

void    run_child(t_cmd *cmd, t_exec *exec, t_shell *shell, int i)
{
    char    *path;
    int     exit_code;
    int     is_empty = 1; //--
    int     j = 0; //---

    if (cmd->redir_error)
        exit_child(exec, exec->cmd_head, 1);
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    setup_io(cmd, exec, i);
    close_pipe_files_child(exec, cmd);
    /* Handle empty command (including "") */
    //    if (!cmd->argv || !cmd->argv[0])//--
    // {
    //     ft_putstr_fd("minishell: : command not found\n", 2);
    //     exit(127);
    // }
    if (!cmd->argv || !cmd->argv[0])
        exit_child(exec, exec->cmd_head, 0);
        /* Check if command is all whitespace */
    while (cmd->argv[0][j]) //---
    {
        if (!ft_isspace(cmd->argv[0][j]))
        {
            is_empty = 0;
            break;
        }
        j++;
    }
    if (is_empty)
    {
        ft_putstr_fd("minishell: ", 2);
        ft_putstr_fd(cmd->argv[0], 2);
        ft_putstr_fd(": command not found\n", 2);
        exit_child(exec, exec->cmd_head, 127);
    }
    // Handle variable expansion as command
    if (cmd->argv[0][0] == '$')
    {
        char *var_name = cmd->argv[0] + 1;
        char *var_value = get_var_value(var_name, shell);
        if (var_value)
        {
            ft_putstr_fd("minishell: ", 2);
            ft_putstr_fd(var_value, 2);
            ft_putstr_fd(": command not found\n", 2);
            free(var_value);
        }
        else
        {
            ft_putstr_fd("minishell: ", 2);
            ft_putstr_fd(cmd->argv[0], 2);
            ft_putstr_fd(": unbound variable\n", 2);
        }
        exit_child(exec, exec->cmd_head, 127);
    }
    if(is_child_builtin(cmd->argv[0]))
        exit_child(exec, exec->cmd_head, exec_builtin_in_child(cmd, shell));
    path = get_cmd_path(cmd->argv[0], shell, exec, exec->cmd_head);
    if (execve(path, cmd->argv, shell->envp) == -1)
    {
        if (errno == ENOENT)
        {
            ft_putstr_fd("minishell: ", 2);
            ft_putstr_fd(cmd->argv[0], 2);
            ft_putstr_fd(": command not found\n", 2);
            exit_code = 127;
        }
        else
        {
            perror(cmd->argv[0]);
            exit_code = 126;
        }
        free(path);
        error_exit(cmd->argv[0], exec, exec->cmd_head, exit_code);
    }
    free(path);
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
        if (cmd->infile != -1)
        {
            close(cmd->infile);
            cmd->infile = -1;
        }
        if (cmd->outfile != -1)
        {
            close(cmd->outfile);
            cmd->outfile = -1;
        }
        cmd = cmd->next;
        i++;
    }
    close_pipe_parent(exec);
}
