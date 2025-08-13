/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 16:54:20 by ashaheen          #+#    #+#             */
/*   Updated: 2025/08/12 14:40:38 by ashaheen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int parse_export_arg(char *arg, char **name, char **value, int *has_eq)
{
	int i;

	i = 0;
	*name = NULL;
	*value = NULL;
	*has_eq = 0;
	while (arg[i] && arg[i] != '=')
		i++;
	if (arg[i] == '=')
	{
		*has_eq = 1;
		*name = ft_substr(arg, 0, i);
		*value = ft_strdup(arg + i + 1);
	}
	else
		*name = ft_strdup(arg);
	if (!*name || (*has_eq && !*value))
	{
		if (*name)
			free(*name);
		if (*value)
			free(*value);
		return (1);
	}
	return (0);
}

char *make_env_pair(char *name, char *value)
{
    char    *tmp;
    char    *pair;

    tmp = NULL;
    pair = NULL;
    tmp = ft_strjoin(name, "=");
    if (!tmp)
        return (NULL);
    if (value)
    {
        pair = ft_strjoin(tmp, value);
        free(tmp);
        if (!pair)
            return (NULL);
    }
    else
        pair = tmp;
    return (pair);
}

int env_set(char ***penvp, char *name, char *value)
{
    int     idx;
	int     n;
	int     i;
	char    *pair;
	char    **newv;

    if (!penvp || !*penvp || !name)
        return (1);
    pair = make_env_pair(name, value);
	if (!pair)
		return (1);
    idx = env_index_of(*penvp, (char *)name);
	if (idx != -1)
	{
		free((*penvp)[idx]);
		(*penvp)[idx] = pair;
		return (0);
	}
    n = 0;
	while ((*penvp)[n])
		n++;
    newv = (char **)malloc(sizeof(char *) * (n + 2));
    if (!newv)
    {
        free(pair);
        return (1);
    }
    i = 0;
	while (i < n)
	{
		newv[i] = (*penvp)[i];
		i++;
	}
	newv[n] = pair;
	newv[n + 1] = NULL;
	free(*penvp);
	*penvp = newv;
	return (0);
}

// ONE HELPER (handles a single arg)
int	export_one(char ***penvp, char *arg)
{
	char	*name;
	char	*value;
	int		has_eq;

	if (parse_export_arg(arg, &name, &value, &has_eq))
		return (1);
	if (!is_valid_identifier(name))
	{
		ft_putstr_fd("minishell: export: ", 2);
		ft_putstr_fd(name, 2);
		ft_putendl_fd(": not a valid identifier", 2);
		free(name);
		free(value);
		return (1);
	}
	if (has_eq)
		env_set(penvp, name, value);
	else if (env_index_of(*penvp, name) == -1)
		env_set(penvp, name, "");
	free(name);
	free(value);
	return (0);
}

int	exec_export(char **argv, t_shell *shell)
{
	int	i;
	int	had_error;

	had_error = 0;
	i = 1;
	if (argv[1] == NULL)
	{
		export_print(shell->envp);
		return (0);
	}
	while (argv[i]) 
	{
		if (export_one(&shell->envp, argv[i]))
			had_error = 1;
		i++;
	}
	if (had_error)
		return (1);
	return (0);
}
