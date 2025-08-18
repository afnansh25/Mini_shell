/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bultin_child.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maabdulr <maabdulr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 20:45:48 by maram             #+#    #+#             */
/*   Updated: 2025/08/12 13:53:38 by maabdulr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int exec_echo(char **av)
{
    int i ;
    int newline;

    i = 1;
    newline = 1;
    if (av[i] && ft_strncmp(av[i], "-n", 3) == 0)
    {
        newline = 0;
        i++;
    }
    while (av[i])
    {
        printf("%s", av[i]);
        if (av[i + 1])
            printf(" ");
        i++;
    }
    if (newline)
        printf("\n");
    return (0);
}

int exec_pwd(char **av)
{
    char *cwd;
    
    if(av[1] != NULL)
    {
        if(av[1][0] == '-')
        {
            ft_putstr_fd("minishell: pwd: ", 2);
            ft_putstr_fd(av[1], 2);
            ft_putstr_fd(": invalid option\n", 2);
        }
    }
    cwd = getcwd(NULL, 0);
    if(cwd == NULL)
    {
        perror("minishell: pwd");
        return 1;
    }
    printf("%s\n", cwd);
    free(cwd);
    return 0;
}

int exec_env(char **av, t_shell *shell)
{
    int i;
    if (av[1] != NULL)
    {
        if (av[1][0] == '-')  // option given
        {
            ft_putstr_fd("minishell: env: ", 2);
            ft_putstr_fd(av[1], 2);
            ft_putstr_fd(": invalid option\n", 2);
    
            return (1); // or 2, but bash uses 125 for option error
        }
        // else
        // {
        //     // No options in minishell -> treat as invalid arg
        //     ft_putstr_fd("minishell: env: ", 2);
        //     ft_putstr_fd(av[1], 2);
        //     ft_putstr_fd(": No such file or directory\n", 2);
        //     return (127);
        // }
    }
    i = 0;
    while(shell->envp[i])
    {
        printf("%s\n", shell->envp[i]);
        i++;
    }
    return(0);
}

