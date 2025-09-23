
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

int	handle_plain_quote(const char *s, int *i, char *rs, int *k)
{
	char	q;
	char	marker;

	if (s[*i] == '\'' || s[*i] == '\"')
	{
		q = s[(*i)++];
		marker = (q == '\'') ? '\x02' : '\x03';
		rs[(*k)++] = marker;
		while (s[*i] && s[*i] != q)
			rs[(*k)++] = s[(*i)++];
		if (s[*i] == q)
		{
			(*i)++;
			rs[(*k)++] = marker;
		}
		return (1);
	}
	return (0);
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
