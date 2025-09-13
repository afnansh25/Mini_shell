/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 14:28:33 by ashaheen          #+#    #+#             */
/*   Updated: 2025/09/13 14:02:24 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*find_path_variable(t_shell *shell)
{
	int	i = 0;

	while (shell->envp[i])
	{
		if (ft_strncmp(shell->envp[i], "PATH=", 5) == 0)
			return (shell->envp[i] + 5);
		i++;
	}
	return (NULL);
}

char	*build_cmd_path(char **paths, char *cmd)
{
	int		i;
	char	*tmp;
	char	*full_path;

	i = 0;
	while (paths[i])
	{
		tmp = ft_strjoin(paths[i], "/");
		full_path = ft_strjoin(tmp, cmd);
		free(tmp);
		if (!full_path)
			return (NULL);
		if (access(full_path, X_OK) == 0)
			return (full_path);
		free(full_path);
		i++;
	}
	return (NULL);
}

char	*handle_explicit_path(char *cmd, t_exec *exec, t_cmd *list)
{
	struct stat	st;
	int			rc;

	rc = access(cmd, F_OK);
	if (rc != 0)
	{
		errno = ENOENT;
		error_exit(cmd, exec, list, 127);
	}
	rc = stat(cmd, &st);
	if (rc == 0 && S_ISDIR(st.st_mode))
	{
		errno = EISDIR;
		error_exit(cmd, exec, list, 126);
	}
	rc = access(cmd, X_OK);
	if (rc == 0)
		return (ft_strdup(cmd));
	error_exit(cmd, exec, list, 126);
	return (NULL);
}

char	*resolve_from_path(char *cmd, t_shell *shell, t_exec *exec, t_cmd *list)
{
	char		*path_str;
	char		**paths;
	char		*full_path;

	path_str = find_path_variable(shell);
	if (!path_str)
	{
		errno = ENOENT;
		error_exit(cmd, exec, list, 127);
	}
	paths = ft_split(path_str, ':');
	if (!paths)
		error_exit("malloc", exec, list, 1);
	full_path = build_cmd_path(paths, cmd);
	free_arr(paths);
	if (!full_path)
	{
		errno = ENOENT;
		error_exit(cmd, exec, list, 127);
	}
	return (full_path);
}

char	*get_cmd_path(char *cmd, t_shell *shell, t_exec *exec, t_cmd *cmd_list)
{
	int	has_slash;

	if (!cmd || !cmd[0])
		return (NULL);
	has_slash = (ft_strchr(cmd, '/') != NULL);
	if (has_slash)
		return (handle_explicit_path(cmd, exec, cmd_list));
	return (resolve_from_path(cmd, shell, exec, cmd_list));
}
