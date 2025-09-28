/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_quote.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maabdulr <maabdulr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 17:20:58 by maram             #+#    #+#             */
/*   Updated: 2025/09/25 19:18:28 by maabdulr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	handle_dollar_quote(const char *s, int *i, char *rs, int *k)
{
	char	q;

	if (s[*i] == '$' && (s[*i + 1] == '\'' || s[*i + 1] == '"'))
	{
		q = s[*i + 1];
		*i += 2;
		while (s[*i] && s[*i] != q)
			rs[(*k)++] = s[(*i)++];
		if (s[*i] == q)
			(*i)++;
		if (s[*i])
			rs[(*k)++] = '\x01';
		return (1);
	}
	return (0);
}

static char	get_marker(char q)
{
	if (q == '\'')
		return ('\x02');
	return ('\x03');
}

int	handle_plain_quote(const char *s, int *i, char *rs, int *k)
{
	char	q;
	char	m;

	if (s[*i] != '\'' && s[*i] != '\"')
		return (0);
	q = s[*i];
	m = get_marker(q);
	(*i)++;
	rs[(*k)++] = m;
	while (s[*i] && s[*i] != q)
	{
		rs[(*k)++] = s[*i];
		(*i)++;
	}
	if (s[*i] == q)
	{
		(*i)++;
		rs[(*k)++] = m;
	}
	return (1);
}

int	handle_backslash(const char *s, int *i, char *rs, int *k)
{
	if (s[*i] == '\\')
	{
		if (s[*i + 1] == '$')
		{
			rs[(*k)++] = s[(*i)++];
			rs[(*k)++] = s[(*i)++];
		}
		else if (s[*i + 1])
		{
			rs[(*k)++] = s[*i + 1];
			*i += 2;
		}
		else
			rs[(*k)++] = s[(*i)++];
		return (1);
	}
	return (0);
}

char	*handle_complex_quotes(const char *s)
{
	int		i;
	int		k;
	char	*rs;

	i = 0;
	k = 0;
	rs = malloc(ft_strlen(s) * 2 + 1);
	if (!rs)
		return (NULL);
	while (s[i])
	{
		if (handle_dollar_quote(s, &i, rs, &k)
			|| handle_plain_quote(s, &i, rs, &k)
			|| handle_backslash(s, &i, rs, &k))
			continue ;
		rs[k++] = s[i++];
	}
	rs[k] = '\0';
	return (rs);
}
