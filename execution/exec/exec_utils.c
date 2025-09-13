/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 13:51:20 by maabdulr          #+#    #+#             */
/*   Updated: 2025/09/10 14:35:33 by maram            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
    i = 0;
    while (cmd[i] && cmd[i] != '=')
    {
        if (!ft_isalnum(cmd[i]) && cmd[i] != '_')
            return (0);
        i++;
    }
    return (cmd[i] == '=' && i > 0);
}

int handle_variable_assignment(char *assignment, t_shell *shell)
{
    char    *equals_pos;
    char    *var_name;
    char    *var_value;
    int     name_len;
    
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
    env_set(&shell->envp, var_name, var_value);
    free(var_name);
    free(var_value);
    return (1);
}
