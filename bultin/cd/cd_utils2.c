/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maabdulr <maabdulr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 18:22:41 by maram             #+#    #+#             */
/*   Updated: 2025/10/13 12:21:17 by maabdulr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*get_oldpwd(t_shell *shell)
{
	char	*oldpwd;

	oldpwd = getcwd(NULL, 0);
	if (!oldpwd)
	{
		oldpwd = dup_env_pwd(shell);
		if (!oldpwd)
			perror("minishell: cd: getcwd");
	}
	return (oldpwd);
}

static int	update_pwd_vars(char *oldpwd, char *newpwd, t_shell *shell)
{
	if (!newpwd)
	{
		update_env_var("OLDPWD", oldpwd, shell);
		perror("minishell: cd: getcwd");
		free(oldpwd);
		return (1);
	}
	set_logical_pwd(NULL, NULL, newpwd, shell);
	update_env_var("OLDPWD", oldpwd, shell);
	return (0);
}

int	exec_cd(char **av, t_shell *shell)
{
	char	*oldpwd;
	char	*newpwd;
	char	*target;
	char	*alloc;
	int		print_newpwd;

	oldpwd = get_oldpwd(shell);
	if (!oldpwd)
		return (1);
	target = resolve_target(av, shell, &print_newpwd, &alloc);
	if (!target)
		return (free(oldpwd), free(alloc), 1);
	if (chdir(target))
		return (cd_perror(target), free(oldpwd), free(alloc), 1);
	newpwd = getcwd(NULL, 0);
	if (update_pwd_vars(oldpwd, newpwd, shell))
		return (free(alloc), 1);
	if (print_newpwd)
		ft_putendl_fd(newpwd, STDOUT_FILENO);
	free(oldpwd);
	free(newpwd);
	free(alloc);
	return (0);
}
