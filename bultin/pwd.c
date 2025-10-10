/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 18:04:39 by maram             #+#    #+#             */
/*   Updated: 2025/10/10 18:17:31 by maram            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


static int      display_logical_pwd(t_shell *shell)
{
        char    *logical_pwd;

        logical_pwd = dup_env_pwd(shell);
        if (!logical_pwd)
                return (perror("minishell: pwd"), 1);
        ft_putendl_fd(logical_pwd, STDOUT_FILENO);
        free(logical_pwd);
        return (0);
}

int             exec_pwd(char **av, t_shell *shell)
{
        char    *cwd;

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
                return (display_logical_pwd(shell));
        ft_putendl_fd(cwd, STDOUT_FILENO);
        free(cwd);
        return (0);
}