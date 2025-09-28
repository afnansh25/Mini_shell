/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils3.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maabdulr <maabdulr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/27 16:54:18 by maabdulr          #+#    #+#             */
/*   Updated: 2025/09/27 16:54:23 by maabdulr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	print_unclosed_error(char q, t_shell *sh)
{
	ft_putstr_fd(
		"minishell: syntax error: unexpected EOF while looking for matching `",
		2);
	ft_putchar_fd(q, 2);
	ft_putstr_fd("'\n", 2);
	sh->exit_code = 258;
}

static int	scan_line(char *line)
{
	int		i;
	char	q;

	i = 0;
	q = '\0';
	while (line[i])
	{
		if (line[i] == '\\' && q != '\'' && line[i + 1])
			i += 2;
		else
		{
			if (!q && (line[i] == '\'' || line[i] == '\"'))
				q = line[i];
			else if (q && line[i] == q)
				q = '\0';
			i++;
		}
	}
	return (q);
}

int	has_unclosed_quote(char *line, t_shell *sh)
{
	char	q;

	q = scan_line(line);
	if (q)
	{
		print_unclosed_error(q, sh);
		return (1);
	}
	return (0);
}
