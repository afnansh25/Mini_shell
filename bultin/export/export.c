/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 16:54:20 by ashaheen          #+#    #+#             */
/*   Updated: 2025/09/14 18:06:40 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	parse_export_arg(char *arg, t_export_arg *out)
{
	int	i;

	out->name = NULL;
	out->value = NULL;
	out->has_eq = 0;
	out->append = 0;
	i = 0;
	while (arg[i] && arg[i] != '=')
		i++;
	if (arg[i] == '=')
	{
		out->has_eq = 1;
		if (set_export_fields(out, arg, i))
			return (1);
	}
	else
	{
		out->name = ft_strdup(arg);
		if (!out->name)
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

int     env_set(char ***penvp, char *name, char *value)
{
    int     idx;
    char    *pair;
    char    **nv;

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
    nv = append_env(*penvp, pair);
    if (!nv)
    {
        free(pair);
        return (1);
    }
    *penvp = nv;
    return (0);
}

int	env_append(char ***penvp, char *name, char *value)
{
	int		idx;
	char	*old;
	char	*joined;
	int		err;

	if (!penvp || !*penvp || !name || !value)
		return (1);
	idx = env_index_of(*penvp, name);
	if (idx == -1)
		return (env_set(penvp, name, value) != 0);
	old = ft_strchr((*penvp)[idx], '=');
	if (old)
		old++;
	else
		old = "";
	joined = ft_strjoin(old, value);
	if (!joined)
		return (1);
	err = env_set(penvp, name, joined);
	free(joined);
	if (err)
		return (1);
	return (0);
}

int	export_one(t_shell *shell, char *arg)
{
	t_export_arg	out;

	if (parse_export_arg(arg, &out))
		return (1);
	if (!is_valid_identifier(out.name))
		return (export_err_id(arg, &out));
	if (out.has_eq)
	{
		if (out.append)
			env_append(&shell->envp, out.name, out.value);
		else
			env_set(&shell->envp, out.name, out.value);
		export_remove(&shell->exp, out.name);
	}
	if (!out.has_eq
		&& env_index_of(shell->envp, out.name) == -1
		&& export_index_of(shell->exp, out.name) == -1)
		export_add(&shell->exp, out.name);
	free(out.name);
	free(out.value);
	return (0);
}

