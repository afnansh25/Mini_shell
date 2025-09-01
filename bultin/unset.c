/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 15:00:02 by ashaheen          #+#    #+#             */
/*   Updated: 2025/08/31 14:31:20 by ashaheen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_valid_identifier(char *s)
{
	int	i;

	if (!s || s[0] == '\0')
		return (0);
	if (!(ft_isalpha((unsigned char)s[0]) || s[0] == '_'))
		return (0);
	i = 1;
	while (s[i])
	{
		if (!(ft_isalnum((unsigned char)s[i]) || s[i] == '_'))
			return (0);
		i++;
	}
	return (1);
}

int env_index_of(char **envp, char *name)
{
    int i;
    int len;
    
    if (!envp || !name)
        return (-1);
    len = ft_strlen(name);
    i = 0;
    while (envp && envp[i])
    {
		if (ft_strncmp(envp[i], name, len) == 0
		&& (envp[i][len] == '=' || envp[i][len] == '\0'))
            return (i);
        i++;
    }
    return (-1);
}

void    env_remove_at(char **envp, int idx)
{
    int i;
    
    if(idx < 0 || !envp[idx])
        return ;
    free(envp[idx]);
    i = idx;
    while(envp[i])
    {
        envp[i] = envp[i + 1];
        i++;
    }
}

void	print_unset_invalid(char *name, int *had_error)
{
	ft_putstr_fd("minishell: unset: '", 2);
	ft_putstr_fd(name, 2);
	ft_putendl_fd("': not a valid identifier", 2);
    *had_error = 1;
}

int	exec_unset(char **argv, t_shell *shell)
{
	int	i;
	int	idx;
	int	had_error;

	had_error = 0;
	i = 1;
	while (argv[i])
	{
		if (!is_valid_identifier(argv[i]))
            print_unset_invalid(argv[i], &had_error);
		else
		{
			export_remove(&shell->exp, argv[i]);
			idx = env_index_of(shell->envp, argv[i]);
			while (idx != -1)
			{
					env_remove_at(shell->envp, idx);
					idx = env_index_of(shell->envp, argv[i]);
			}
		}
		i++;
	}
	if (had_error)
		return (1);
	return (0);
}


