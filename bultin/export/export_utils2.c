/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/14 11:39:25 by codespace         #+#    #+#             */
/*   Updated: 2025/09/14 18:06:52 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	process_export_token(t_shell *shell, char *arg, int *had_error)
{
	char	*bang;

	if (arg[0] == '-')
	{
		ft_putstr_fd("minishell: export: ", 2);
		ft_putchar_fd('-', 2);
		if (arg[1])
			ft_putchar_fd(arg[1], 2);
		ft_putendl_fd(": invalid option", 2);
		ft_putendl_fd("export: usage: export [-nf] [name[=value] ...] or export -p", 2);
		return (2);
	}
	bang = ft_strchr(arg, '!');
	if (bang)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(bang, 2);
		ft_putendl_fd(": event not found", 2);
		return (1);
	}
	if (export_one(shell, arg))
		*had_error = 1;
	return (-1);
}

int	env_grow_append(char ***pexp, int n, char *name)
{
	char	**newv;
	int		i;

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
	{
		free(newv);
		return (1);
	}
	newv[n + 1] = NULL;
	free(*pexp);
	*pexp = newv;
	return (0);
}

int	set_export_fields(t_export_arg *out, char *arg, int i)
{
	if (i > 0 && arg[i - 1] == '+')
	{
		out->append = 1;
		out->name = ft_substr(arg, 0, i - 1);
	}
	else
		out->name = ft_substr(arg, 0, i);
	out->value = ft_strdup(arg + i + 1);
	if (!out->name || !out->value)
		return (1);
	return (0);
}

int	export_err_id(char *arg, t_export_arg *o)
{
	ft_putstr_fd("minishell: export: `", 2);
	ft_putstr_fd(arg, 2);
	ft_putendl_fd("': not a valid identifier", 2);
	if (o->name)
		free(o->name);
	if (o->value)
		free(o->value);
	return (1);
}

char    **append_env(char **envp, char *entry)
{
    char    **newv;
    int      n;
    int      i;

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
