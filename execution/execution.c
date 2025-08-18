/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 13:51:20 by maabdulr          #+#    #+#             */
/*   Updated: 2025/08/18 16:47:06 by ashaheen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int is_child_builtin(char *cmd)
{
    if(ft_strncmp(cmd, "echo", 5) == 0)
        return (1);
    if(ft_strncmp(cmd, "pwd", 4) == 0)
        return (1);
    if(ft_strncmp(cmd, "env", 4) == 0)
        return (1);
    return(0);
}
int is_parent_builtin(char *cmd)
{
    if (ft_strncmp(cmd, "cd", 3) == 0)
        return (1);
    if (ft_strncmp(cmd, "export", 7) == 0)
        return (1);
    if (ft_strncmp(cmd, "exit", 5) == 0)
        return (1);
    if (ft_strncmp(cmd, "unset", 6) == 0)
        return (1);
    return(0);
}
int exec_builtin_in_parent(t_cmd *cmd, t_shell *shell)
{
    int interactive;
    int status;
        
    interactive = isatty(0);
    status = 0;
    if (ft_strncmp(cmd->argv[0], "cd", 3) == 0)
        return (exec_cd(cmd->argv, shell));
    if (ft_strncmp(cmd->argv[0], "export", 7) == 0)
        return (exec_export(cmd->argv, shell));
    if (!ft_strncmp(cmd->argv[0], "exit", 5))
        return (exec_exit(cmd->argv, shell, interactive));
    if (ft_strncmp(cmd->argv[0], "unset", 6) == 0)
        return (exec_unset(cmd->argv, shell));
    return(status);
}

int exec_builtin_in_child(t_cmd *cmd, t_shell *shell)
{
    if (ft_strncmp(cmd->argv[0], "echo", 5) == 0)
        return (exec_echo(cmd->argv));
    if (ft_strncmp(cmd->argv[0], "pwd", 4) == 0)
        return (exec_pwd(cmd->argv));
    if (ft_strncmp(cmd->argv[0], "env", 4) == 0)
        return (exec_env(cmd->argv, shell));
    return (0);
}

int count_cmds(t_cmd *cmd)
{
    int count;
    
    count = 0;
    while (cmd)
    {
        count++;
        cmd = cmd->next;
    } 
    return (count);
}
void wait_all_children(t_exec *exec, t_shell *shell)
{
    int i;
    int st;

    i = 0;
    while (i < exec->cmd_count)
    {
        if (waitpid(exec->pids[i], &st, 0) > 0)
        {
            // record exit code of the LAST child (bash behavior)
            if (i == exec->cmd_count - 1)
            {
                if (WIFEXITED(st))
                    shell->exit_code = WEXITSTATUS(st);
                else if (WIFSIGNALED(st))
                    shell->exit_code = 128 + WTERMSIG(st); // e.g. 130 on SIGINT
            }
        }
        i++;
    }
}

void execute_pipeline(t_cmd *cmd_list, t_shell *shell)
{
	t_exec  *exec;

	handle_all_heredocs(cmd_list, shell);
    if (shell->exit_code == 130)
        return;
	if (cmd_list && cmd_list->next == NULL
        && is_parent_builtin(cmd_list->argv[0]))
	    {
            shell->exit_code = exec_builtin_in_parent(cmd_list, shell);
            return;
        }
    exec = init_exec_struct(cmd_list, shell);
    fork_and_execute_all(cmd_list, exec, shell);
    wait_all_children(exec, shell);
    free_exec_data(exec);
}
