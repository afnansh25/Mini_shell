/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 18:12:22 by maram             #+#    #+#             */
/*   Updated: 2025/09/26 18:15:09 by maram            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	skip_to_closing(char *s, int *i, char q)
{
	(*i)++;
	while (s[*i] && s[*i] != q)
		(*i)++;
	if (!s[*i])
		return (0);
	(*i)++;
	return (1);
}

static int	handle_quote(char *s, int *i, t_quote_type *t, char q)
{
	if (*t == NO_QUOTE)
	{
		if (q == '\'')
			*t = SINGLE_QUOTE;
		else
			*t = DOUBLE_QUOTE;
	}
	else if ((*t == SINGLE_QUOTE && q != '\'')
		|| (*t == DOUBLE_QUOTE && q != '\"'))
		return (0);
	return (skip_to_closing(s, i, q));
}

t_quote_type	determine_quote_type(char *raw)
{
	int				i;
	t_quote_type	type;

	i = 0;
	type = (NO_QUOTE);
	while (raw[i])
	{
		if (raw[i] == '\'' || raw[i] == '\"')
		{
			if (!handle_quote(raw, &i, &type, raw[i]))
				return (NO_QUOTE);
		}
		else
			return (NO_QUOTE);
	}
	return (type);
}
