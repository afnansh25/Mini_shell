/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 12:28:23 by ashaheen          #+#    #+#             */
/*   Updated: 2025/09/24 14:24:14 by maram            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_numeric_str(char *s)
{
	int	i;

	i = 0;
	if (!s || s[i] == '\0')
		return (0);
	if (s[i] == '+' || s[i] == '-')
	{
		i++;
		if (s[i] == '\0')
			return (0);
	}
	while (s[i])
	{
		if (ft_isdigit((unsigned char)s[i]) == 0)
			return (0);
		i++;
	}
	return (1);
}

unsigned char	normalize_exit_code(long long n)
{
	n %= 256;
	if (n < 0)
		n += 256;
	return ((unsigned char)n);
}

void	cleanup_and_exit(t_shell *shell, t_cmd *cmd, unsigned char status)
{
	free_cmd_list(cmd);
	free_envp(shell->envp);
	free_arr(shell->exp);
	clear_history();
	exit(status);
}

static int	exit_error(int kind, char *arg, t_shell *shell, t_cmd *cmd)
{
	if (kind == 0)
	{
		ft_putstr_fd("minishell: exit: ", 2);
		ft_putstr_fd(arg, 2);
		ft_putendl_fd(": numeric argument required", 2);
		cleanup_and_exit(shell, cmd, 255);
		return (0);
	}
	ft_putendl_fd("minishell: exit: too many arguments", 2);
	shell->exit_code = 1;
	return (1);
}

int	exec_exit(t_cmd *cmd, t_shell *shell, int interactive)
{
	char		**argv;
	long long	n;

	argv = cmd->argv;
	if (interactive)
		ft_putendl_fd("exit", 2);
	if (argv[1] && (!is_numeric_str(argv[1])
			|| !is_within_long_long(argv[1])))
		return (exit_error(0, argv[1], shell, cmd));
	if (argv[1] && argv[2])
		return (exit_error(1, NULL, shell, NULL));
	if (!argv[1])
	{
		cleanup_and_exit(shell, cmd, shell->exit_code);
		return (0);
	}
	n = ft_atoll(argv[1]);
	cleanup_and_exit(shell, cmd, normalize_exit_code(n));
	return (0);
}
