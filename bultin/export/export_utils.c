/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 14:39:59 by ashaheen          #+#    #+#             */
/*   Updated: 2025/09/04 16:44:22 by ashaheen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int   export_index_of(char **exp, char *name)
{
    int i;

    if (!exp || !name)
        return (-1);
    i = 0;
    while (exp[i])
    {
        if (ft_strncmp(exp[i], name, ft_strlen(name) + 1) == 0)
            return (i);
        i++;
    }
    return (-1);
}

int   export_add(char ***pexp, char *name)
{
    int     n;
    int     i;
    char    **newv;

    if (!name)
        return (1);
    if (export_index_of(*pexp, name) != -1)
        return (0);
    n = 0;
    if (*pexp)
        while ((*pexp)[n])
            n++;
    newv = malloc(sizeof(char *) * (n + 2));
    if (!newv)
        return (1);
    i = 0;
    while (i < n)
    {
        newv[i] = (*pexp)[i];
        i++;
    }
    newv[n] = ft_strdup(name);
    if (!newv[n])
        return (free(newv), 1);
    newv[n + 1] = NULL;
    free(*pexp);
    *pexp = newv;
    return (0);
}

void   export_remove(char ***pexp, char *name)
{
    int idx;
    int i;

    if (!pexp || !*pexp)
        return ;
    idx = export_index_of(*pexp, name);
    if (idx == -1)
        return ;
    free((*pexp)[idx]);
    i = idx;
    while ((*pexp)[i])
    {
        (*pexp)[i] = (*pexp)[i + 1];
        i++;
    }
}

void	export_print(char **envp, char **exp)
{
	char	**copy;
	int		i;

	copy = join_env_and_exp(envp, exp);
	if (!copy)
		return ;
	sort_env_ptrs(copy);
	i = 0;
	while (copy[i])
	{
		print_decl(copy[i]);
		i++;
	}
	free(copy);
}

int	exec_export(char **argv, t_shell *shell)
{
	int	i;
	int	had_error;

	had_error = 0;
	i = 1;
	if (argv[1] == NULL)
	{
		export_print(shell->envp, shell->exp);
		return (0);
	}
	while (argv[i]) 
	{
		if (export_one(shell, argv[i]))
			had_error = 1;
		i++;
	}
	if (had_error)
		return (1);
	return (0);
}
