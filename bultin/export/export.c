/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 16:54:20 by ashaheen          #+#    #+#             */
/*   Updated: 2025/08/30 12:41:22 by maram            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//1
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
	if (!value)
        return (ft_strdup(name));
    tmp = ft_strjoin(name, "=");
    if (!tmp)
        return (NULL);
    pair = ft_strjoin(tmp, value);
    free(tmp);
    if (!pair)
        return (NULL);
    return (pair);
}
/* helper: append PAIR to *penvp (realloc array by +1) */
// static int	v(char ***penvp, char *pair) //new
// {
// 	char	**old;
// 	char	**newv;
// 	int		n;
// 	int		i;

// 	old = *penvp;
// 	n = 0;
// 	while (old && old[n])
// 		n++;
// 	newv = (char **)malloc(sizeof(char *) * (n + 2));
// 	if (!newv)
// 		return (1);
// 	i = 0;
// 	while (i < n)
// 	{
// 		newv[i] = old[i];
// 		i++;
// 	}
// 	newv[n] = pair;
// 	newv[n + 1] = NULL;
// 	free(old);
// 	*penvp = newv;
// 	return (0);
// }

/* minishell.h already has:
** char **append_env(char **envp, char *entry);
*/

/* realloc envp by +1 and append entry; returns new envp or NULL on fail */
char	**append_env(char **envp, char *entry)
{
	char	**newv;
	int		n;
	int		i;

	n = 0;
	while (envp && envp[n])
		n++;
	newv = (char **)malloc(sizeof(char *) * (n + 2));
	if (!newv)
		return (NULL);
	i = 0;
	while (i < n)
	{
		newv[i] = envp[i];
		i++;
	}
	newv[n] = entry;
	newv[n + 1] = NULL;
	free(envp);
	return (newv);
}

int	env_set(char ***penvp, char *name, char *value)
{
	int		idx;
	char	*pair;
	char	**nv;

	if (!penvp || !*penvp || !name)
		return (1);
	pair = make_env_pair(name, value);
	if (!pair)
		return (1);
	idx = env_index_of(*penvp, name);
	if (idx != -1)
	{
		free((*penvp)[idx]);
		(*penvp)[idx] = pair;
		return (0);
	}
	nv = append_env(*penvp, pair);   /* ✅ real call */
	if (!nv)
	{
		free(pair);
		return (1);
	}
	*penvp = nv;
	return (0);
}


// ONE HELPER (handles a single arg)
int export_one(t_shell *shell, char *arg)
{
	char	*name;
	char	*value;
	int		has_eq;

	if (parse_export_arg(arg, &name, &value, &has_eq))
		return (1);
	if (!is_valid_identifier(name))
	{
		ft_putstr_fd("minishell: export: `", 2);
		ft_putstr_fd(arg, 2);
		ft_putendl_fd("': not a valid identifier", 2);
		free(name);
		free(value);
		return (1);
	}
    if (has_eq && !env_set(&shell->envp, name, value))
        {export_remove(&shell->exp, name);}
    else if (!has_eq && env_index_of(shell->envp, name) == -1
        && export_index_of(shell->exp, name) == -1)
        {export_add(&shell->exp, name);}
	free(name);
	free(value);
	return (0);
}