/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 14:39:59 by ashaheen          #+#    #+#             */
/*   Updated: 2025/09/24 15:46:13 by maram            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	export_index_of(char **exp, char *name)
{
	int	i;

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

int	export_add(char ***pexp, char *name)
{
	int	n;

	if (!name)
		return (1);
	if (export_index_of(*pexp, name) != -1)
		return (0);
	n = 0;
	if (*pexp)
		while ((*pexp)[n])
			n++;
	return (env_grow_append(pexp, n, name));
}

void	export_remove(char ***pexp, char *name)
{
	int	idx;
	int	i;

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
	char		**copy;
	int			i;

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
	int	rc;

	had_error = 0;
	i = 1;
	if (argv[1] == NULL)
	{
		export_print(shell->envp, shell->exp);
		return (0);
	}
	while (argv[i])
	{
		rc = process_export_token(shell, argv[i], &had_error);
		if (rc != -1)
			return (rc);
		i++;
	}
	if (had_error)
		return (1);
	return (0);
}
