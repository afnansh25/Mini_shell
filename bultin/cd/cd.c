/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 13:54:17 by maabdulr          #+#    #+#             */
/*   Updated: 2025/10/10 18:23:08 by maram            ###   ########.fr       */
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
		ft_putstr_fd("bash: cd: HOME not set\n", 2);
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

char	*resolve_target(char **av, t_shell *shell, int *print_newpwd,
	char **alloc)
{
	char	*t;

	*print_newpwd = 0;
	*alloc = NULL;
	if (cd_home_case(av))
	{
		t = get_env_value("HOME", shell);
		if (!t || t[0] == '\0')
			return (ft_putstr_fd("minishell: cd: HOME not set\n", 2), NULL);
		return (t);
	}
	if (av[1][0] == '-' && av[1][1] == '\0')
        {
                t = get_env_value("OLDPWD", shell);
                if (!t || t[0] == '\0')
                {
                        ft_putstr_fd("minishell: cd: OLDPWD not set\n", 2);
                        return (NULL);
                }
                *print_newpwd = 1;
                return (t);
        }
	if (av[1][0] == '~')
		return (expand_tilde(av[1], shell, alloc));
	return (av[1]);
}

void	set_logical_pwd(char **av, char *target_dir, char *newpwd,
	t_shell *shell)
{
	if (!av || !av[1])
	{
		if (target_dir)
			update_env_var("PWD", target_dir, shell);
		else
			update_env_var("PWD", newpwd, shell);
		return ;
	}
	if ((av[1][0] == '-' && av[1][1] == '-' && av[1][2] == '\0')
		|| (av[1][0] == '\0'))
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
