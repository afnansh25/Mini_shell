/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 18:04:39 by maram             #+#    #+#             */
/*   Updated: 2025/09/24 16:28:57 by maram            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	exec_pwd(char **av)
{
	char	*cwd;

	if (av[1] != NULL)
	{
		if (av[1][0] == '-')
		{
			ft_putstr_fd("minishell: pwd: ", 2);
			ft_putstr_fd(av[1], 2);
			ft_putstr_fd(": invalid option\n", 2);
		}
	}
	cwd = getcwd(NULL, 0);
	if (cwd == NULL)
	{
		perror("minishell: pwd");
		return (1);
	}
	ft_putendl_fd(cwd, STDOUT_FILENO);
	free(cwd);
	return (0);
}
