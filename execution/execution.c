/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 13:51:20 by maabdulr          #+#    #+#             */
/*   Updated: 2025/09/10 14:35:33 by maram            ###   ########.fr       */
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
    if(ft_strncmp(cmd, "export", 7) == 0)
        return (1);
    if(ft_strncmp(cmd, "cd", 3) == 0)
        return (1);
    if(ft_strncmp(cmd, "unset", 6) == 0)
        return (1);
    if(ft_strncmp(cmd, "exit", 5) == 0)
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

int is_variable_assignment(char *cmd)
{
    int i;
    
    if (!cmd || !cmd[0])
        return (0);
    
    // Check if it contains an equals sign
    i = 0;
    while (cmd[i] && cmd[i] != '=')
    {
        // Variable name can only contain alphanumeric characters and underscores
        if (!ft_isalnum(cmd[i]) && cmd[i] != '_')
            return (0);
        i++;
    }
    
    // Must have an equals sign and at least one character before it
    return (cmd[i] == '=' && i > 0);
}

int handle_variable_assignment(char *assignment, t_shell *shell)
{
    char *equals_pos;
    char *var_name;
    char *var_value;
    int name_len;
    
    equals_pos = ft_strchr(assignment, '=');
    if (!equals_pos)
        return (0);
    
    name_len = equals_pos - assignment;
    var_name = ft_substr(assignment, 0, name_len);
    var_value = ft_strdup(equals_pos + 1);
    
    if (!var_name || !var_value)
    {
        free(var_name);
        free(var_value);
        return (0);
    }
    
    // Set the environment variable
    env_set(&shell->envp, var_name, var_value);
    
    free(var_name);
    free(var_value);
    return (1);
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
        return (exec_exit(cmd, shell, interactive));
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
    if (ft_strncmp(cmd->argv[0], "export", 7) == 0)
        return (exec_export(cmd->argv, shell));
    if (ft_strncmp(cmd->argv[0], "cd", 3) == 0)
        return (exec_cd(cmd->argv, shell));
    if (ft_strncmp(cmd->argv[0], "unset", 6) == 0)
        return (exec_unset(cmd->argv, shell));
    if (ft_strncmp(cmd->argv[0], "exit", 5) == 0)
        return (exec_exit(cmd, shell, 0));
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

    if (!cmd_list)
        {return;}
    if (handle_all_heredocs(cmd_list, shell))
        {return;}
    
    // Check for variable assignment (single command with assignment)
    if (cmd_list && cmd_list->next == NULL && cmd_list->argv && cmd_list->argv[0])
    {
        if (is_variable_assignment(cmd_list->argv[0]))
        {
            if (handle_variable_assignment(cmd_list->argv[0], shell))
                shell->exit_code = 0;
            else
                shell->exit_code = 1;
            return;
        }
    }
    
	if (cmd_list && cmd_list->next == NULL
        && cmd_list->argv && is_parent_builtin(cmd_list->argv[0]))
	{
        shell->exit_code = exec_builtin_in_parent(cmd_list, shell);
        return;
    }
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    exec = init_exec_struct(cmd_list, shell);
    fork_and_execute_all(cmd_list, exec, shell);
    wait_all_children(exec, shell);
    free_exec_data(exec);
    setup_signals();
}
