/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 17:59:14 by maram             #+#    #+#             */
/*   Updated: 2025/09/24 16:28:18 by maram            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*resolve_cmd_path_nf(char *cmd, t_shell *shell)
{
	char	*path_str;
	char	**paths;
	char	*full;

	if (!cmd || !cmd[0])
		return (NULL);
	if (ft_strchr(cmd, '/'))
	{
		if (access(cmd, X_OK) == 0)
			return (ft_strdup(cmd));
		return (NULL);
	}
	path_str = find_path_variable(shell);
	if (!path_str)
		return (NULL);
	paths = ft_split(path_str, ':');
	if (!paths)
		return (NULL);
	full = build_cmd_path(paths, cmd);
	free_arr(paths);
	return (full);
}

static int	env_bad_opt(char *opt)
{
	ft_putstr_fd("minishell: env: ", 2);
	ft_putstr_fd(opt, 2);
	ft_putstr_fd(": invalid option\n", 2);
	return (1);
}

static int	env_run_prog(char **av, t_shell *shell)
{
	char	*path;

	path = resolve_cmd_path_nf(av[1], shell);
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

int	exec_env(char **av, t_shell *shell)
{
	int	i;

	if (av[1] != NULL)
	{
		if (av[1][0] == '-')
			return (env_bad_opt(av[1]));
		return (env_run_prog(av, shell));
	}
	i = 0;
	while (shell->envp[i])
	{
		ft_putendl_fd(shell->envp[i], STDOUT_FILENO);
		i++;
	}
	return (0);
}

char	*get_env_value(char *name, t_shell *shell)
{
	size_t	name_len;
	int		i;

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
