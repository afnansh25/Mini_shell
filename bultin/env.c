/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 17:59:14 by maram             #+#    #+#             */
/*   Updated: 2025/09/09 19:57:07 by maram            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char *find_cmd_in_path(char *cmd, char **envp)
{
    char    *path_str;
    char    **paths;
    char    *tmp;
    char    *full_path;
    int     i;

    if (ft_strchr(cmd, '/'))
    {
        if (access(cmd, X_OK) == 0)
            return (ft_strdup(cmd));
        return (NULL);
    }
    path_str = NULL;
    i = 0;
    while (envp && envp[i])
    {
        if (ft_strncmp(envp[i], "PATH=", 5) == 0)
        {
            path_str = envp[i] + 5;
            break ;
        }
        i++;
    }
    if (!path_str)
        return (NULL);
    paths = ft_split(path_str, ':');
    if (!paths)
        return (NULL);
    i = 0;
    while (paths[i])
    {
        tmp = ft_strjoin(paths[i], "/");
        if (!tmp)
            return (free_arr(paths), NULL);
        full_path = ft_strjoin(tmp, cmd);
        free(tmp);
        if (!full_path)
            return (free_arr(paths), NULL);
        if (access(full_path, X_OK) == 0)
            return (free_arr(paths), full_path);
        free(full_path);
        i++;
    }
    free_arr(paths);
    return (NULL);
}

int exec_env(char **av, t_shell *shell)
{
    int     i;
    char    *path;

    if (av[1] != NULL)
    {
        if (av[1][0] == '-')
        {
            ft_putstr_fd("minishell: env: ", 2);
            ft_putstr_fd(av[1], 2);
            ft_putstr_fd(": invalid option\n", 2);
            return (1);
        }
        path = find_cmd_in_path(av[1], shell->envp);
        if (!path)
        {
            ft_putstr_fd("env: ", 2);
            ft_putstr_fd(av[1], 2);
            ft_putstr_fd(": No such file or directory\n", 2);
            return (127);
        }
        execve(path, av + 1, shell->envp);
        perror(av[1]);
        free(path);
        return (126);
    }
    i = 0;
    while (shell->envp[i])
    {
        printf("%s\n", shell->envp[i]);
        i++;
    }
    return (0);
}

char *get_env_value(char *name, t_shell *shell)   
{
    size_t  name_len;
    int     i;

    name_len = ft_strlen(name);
    i = 0;
    while (shell->envp && shell->envp[i])
    {
        if (ft_strncmp(shell->envp[i], name, name_len) == 0
            && shell->envp[i][name_len] == '=')
            return (shell->envp[i] + name_len + 1);
        i++;
    }
    return (NULL);
}

int  env_count(char **env)
{
    int i;

    i = 0;
    while (env && env[i])
        i++;
    return (i);
}

void    free_envp(char **env)
{
    int i;

    if (!env)
        return;
    i = 0;
    while (env[i])
    {
        free(env[i]);
        i++;
    }
    free(env);
}
//---======999
void init_shlvl(char ***penvp)
{
	int     idx;
	char    *s;
	int     lvl;
	char    *num;

	if (!penvp || !*penvp)
		return ;
	idx = env_index_of(*penvp, "SHLVL");
	if (idx == -1)
		return ((void)env_set(penvp, "SHLVL", "1"));
	s = (*penvp)[idx] + 6;
	lvl = (is_numeric_str(s) ? ft_atoi(s) : 0);
	if (lvl < 0) lvl = 0;
	lvl += 1;
	num = ft_itoa(lvl);
	if (!num) return ;
	env_set(penvp, "SHLVL", num);
	free(num);
}

void init_uid(char ***penvp)
{
    uid_t   uid;
    char    *uid_str;

    if (!penvp || !*penvp)
        return ;
    uid = getuid();
    uid_str = ft_itoa(uid);
    if (!uid_str)
        return ;
    env_set(penvp, "UID", uid_str);
    free(uid_str);
}