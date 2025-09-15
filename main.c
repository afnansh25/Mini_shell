/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 14:29:10 by ashaheen          #+#    #+#             */
/*   Updated: 2025/09/15 15:04:55 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	free_partial_env(char **dst, int i)
{
	while (i > 0)
	{
		i--;
		free(dst[i]);
	}
	free(dst);
}

char	**dup_envp(char **src)
{
	char	**dst;
	int		n;
	int		i;

	n = env_count(src);
	dst = (char **)malloc(sizeof(char *) * (n + 1));
	if (!dst)
		return (NULL);
	i = 0;
	while (i < n)
	{
		dst[i] = ft_strdup(src[i]);
		if (!dst[i])
		{
			free_partial_env(dst, i);
			return (NULL);
		}
		i++;
	}
	dst[n] = NULL;
	return (dst);
}

static int	shell_init(t_shell *sh, char **envp)
{
	char	**dup;

	dup = dup_envp(envp);
	if (!dup)
	{
		ft_putstr_fd("minishell: failed to duplicate environment\n", 2);
		return (1);
	}
	sh->envp = dup;
	init_shlvl(&sh->envp);
	init_uid(&sh->envp);
	sh->exit_code = 0;
	sh->exp = NULL;
	return (0);
}

int	main(int ac, char **av, char **envp)
{
	t_shell	shell;

	(void)av;
	if (ac > 1)
	{
		ft_putstr_fd("You Should Write ./minishell\n", 1);
		return (0);
	}
	if (shell_init(&shell, envp))
		return (1);
	repl_loop(&shell);
	shell_shutdown(&shell);
	return (shell.exit_code);
}
