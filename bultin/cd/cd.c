/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 13:54:17 by maabdulr          #+#    #+#             */
/*   Updated: 2025/08/25 18:04:16 by maram            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*expand_tilde(char *arg, t_shell *shell, char **alloc)
{
	char	*home;

	*alloc = NULL;
	home = get_env_value("HOME", shell);
	if (!home || home[0] == '\0')
	{
		ft_putstr_fd("minishell: cd: HOME not set\n", 2);
		return (NULL);
	}
	if (arg[1] == '\0')
		return (home);
	if (arg[1] == '/')
	{
		*alloc = ft_strjoin(home, arg + 1);
		if (!*alloc)
			return (NULL);
		return (*alloc);
	}
	return (arg);
}
/* home/no-arg check split to keep resolve_target < 25 lines */
int	cd_home_case(char **av)
{
	if (!av[1])
		return (1);
	if (av[1][0] == '-' && av[1][1] == '-' && av[1][2] == '\0')
		return (1);
	if (av[1][0] == '\0')
		return (1);
	return (0);
}

char	*resolve_target(char **av, t_shell *shell, int *print_newpwd, char **alloc)
{
	char	*t;

	*print_newpwd = 0;
	*alloc = NULL;
	if (cd_home_case(av))
	{
		t = get_env_value("HOME", shell);
		if (!t || t[0] == '\0')
			return (ft_putstr_fd("minishell: cd: HOME not set\n", 2),NULL);
		return (t);
	}
	if (av[1][0] == '-' && av[1][1] == '\0')
	{
		t = get_env_value("OLDPWD", shell);
		if (!t)
			ft_putstr_fd("minishell: cd: OLDPWD not set\n", 2);
		else
			*print_newpwd = 1;
		return (t);
	}
	if (av[1][0] == '~')
		return (expand_tilde(av[1], shell, alloc));
	return (av[1]);
}


void	set_logical_pwd(char **av, char *target_dir, char *newpwd, t_shell *shell)
{
	if (!av[1] || (av[1][0] == '-' && av[1][1] == '-' && av[1][2] == '\0')
		|| (av[1] && av[1][0] == '\0'))
		update_env_var("PWD", target_dir, shell);
	else if (av[1][0] == '-' && av[1][1] == '\0')
		update_env_var("PWD", target_dir, shell);
	else if (av[1][0] == '/')
		update_env_var("PWD", av[1], shell);
	else if (av[1][0] == '~')
		update_env_var("PWD", target_dir, shell);
	else
		update_env_var("PWD", newpwd, shell);
}

int	exec_cd(char **av, t_shell *shell)
{
	char	*target_dir;
	char	*oldpwd;
	char	*newpwd;
	char	*alloc;
	int		print_newpwd;

	oldpwd = getcwd(NULL, 0);
	if (!oldpwd)
		return (perror("minishell: cd: getcwd"), 1);
	target_dir = resolve_target(av, shell, &print_newpwd, &alloc);
	if (!target_dir)
		return (free(oldpwd), 1);
	if (chdir(target_dir) != 0)
		return (cd_perror(target_dir), free(oldpwd), free(alloc), 1);
	newpwd = getcwd(NULL, 0);
	if (!newpwd)
		return (update_env_var("OLDPWD", oldpwd, shell),
			perror("minishell: cd: getcwd"), free(oldpwd), free(alloc), 1);
	update_env_var("OLDPWD", oldpwd, shell);
	set_logical_pwd(av, target_dir, newpwd, shell);
	if (print_newpwd)
		printf("%s\n", newpwd);
	return (free(oldpwd), free(newpwd), free(alloc), 0);
}
