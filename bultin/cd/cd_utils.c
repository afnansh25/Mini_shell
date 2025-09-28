/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 17:57:06 by maram             #+#    #+#             */
/*   Updated: 2025/09/26 18:05:30 by maram            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*build_entry(char *name, char *value)
{
	char	*tmp;
	char	*res;

	tmp = ft_strjoin(name, "=");
	if (!tmp)
		return (NULL);
	if (value)
		res = ft_strjoin(tmp, value);
	else
		res = ft_strjoin(tmp, "");
	free(tmp);
	return (res);
}

int	find_key(char **envp, char *name)
{
	int		i;
	int		n;

	n = (int)ft_strlen(name);
	i = 0;
	while (envp && envp[i])
	{
		if (ft_strncmp(envp[i], name, n) == 0 && envp[i][n] == '=')
			return (i);
		i++;
	}
	return (i);
}

void	update_env_var(char *name, char *value, t_shell *shell)
{
	int		i;
	char	*entry;

	if (!name || !shell)
		return ;
	entry = build_entry(name, value);
	if (!entry)
		return ;
	i = find_key(shell->envp, name);
	if (shell->envp && shell->envp[i])
	{
		free(shell->envp[i]);
		shell->envp[i] = entry;
	}
	else
	{
		shell->envp = append_env(shell->envp, entry);
		if (!shell->envp)
			free(entry);
	}
}

void	cd_perror(char *path)
{
	char	*tmp;

	tmp = ft_strjoin("minishell: cd: ", path);
	if (tmp)
	{
		perror(tmp);
		free(tmp);
	}
	else
		perror("minishell: cd");
}

char	*dup_env_pwd(t_shell *shell)
{
	char	*pwd;

	pwd = get_env_value("PWD", shell);
	if (!pwd)
		return (NULL);
	return (ft_strdup(pwd));
}
