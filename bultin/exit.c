/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maabdulr <maabdulr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 12:28:23 by ashaheen          #+#    #+#             */
/*   Updated: 2025/08/27 08:41:09 by maabdulr         ###   ########.fr       */
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
long long	ft_atoll(const char *s)
{
	int         i;
	int         sign;
	long long   n;

	i = 0;
	sign = 1;
	n = 0;
	if (s[i] == '+' || s[i] == '-')
	{
		if (s[i] == '-')
			sign = -1;
		i++;
	}
	while (s[i] && s[i] >= '0' && s[i] <= '9')
	{
		n = n * 10 + (s[i] - '0');
		i++;
	}
	return (n * sign);
}

unsigned char	normalize_exit_code(long long n)
{
	n %= 256;
	if (n < 0)
		n += 256;
	return ((unsigned char)n);
}

int	exec_exit(char **argv, t_shell *shell, int interactive)
{
	long long	n;
	int			argc;
	unsigned char	status;

	argc = 0;
	while (argv[argc])
		argc++;
	if (interactive)
		ft_putendl_fd("exit", 2);
	if (argc >= 2 && !is_numeric_str(argv[1]))
	{
		ft_putstr_fd("minishell: exit: ", 2);
		ft_putstr_fd(argv[1], 2);
		ft_putendl_fd(": numeric argument required", 2);
		exit(255);
	}
	if (argc > 2)
	{
		ft_putendl_fd("minishell: exit: too many arguments", 2);
		shell->exit_code = 1;
		return (1);
	}
	if (argc == 1)
		exit(shell->exit_code);
	n = ft_atoll(argv[1]);
	status = normalize_exit_code(n);
	exit(status);
}

