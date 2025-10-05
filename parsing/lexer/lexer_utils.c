/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 11:25:15 by ashaheen          #+#    #+#             */
/*   Updated: 2025/10/05 17:29:46 by ashaheen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	*new_token(char *token, t_token_type type, t_quote_type quote)
{
	t_token	*new_token;

	new_token = malloc(sizeof(t_token));
	if (!new_token)
		return (NULL);
	new_token->value = token;
	new_token->type = type;
	new_token->quote = quote;
	new_token->ambiguous = 0;
	new_token->next = NULL;
	return (new_token);
}

void	token_add_back(t_token **token, t_token *new)
{
	t_token	*last;

	if (!token || !new)
		return ;
	if (!*token)
	{
		*token = new;
		return ;
	}
	last = *token;
	while (last->next != NULL)
		last = last->next;
	last->next = new;
}

int	is_newline_case(char *line, int i)
{
	if (!line[i + 1])
		return (1);
	if (line[i] == '<' && line[i + 1] == '>' && !line[i + 2])
		return (1);
	if (line[i] == '<' && line[i + 1] == '<' && line[i + 2] == '<'
		&& !line[i + 3])
		return (1);
	if (line[i] == '>' && line[i + 1] == '>' && line[i + 2] == '>'
		&& !line[i + 3])
		return (1);
	return (0);
}

void	build_token(char *line, int i, char token[3])
{
	if ((line[i] == '|' && line[i + 1] == '|')
		|| (line[i] == '<' && line[i + 1] == '<' && line[i + 2] == '<')
		|| (line[i] == '>' && line[i + 1] == '>' && line[i + 2] == '>'))
	{
		token[0] = line[i];
		token[1] = '\0';
		return ;
	}
	if (line[i + 1] == line[i])
	{
		token[0] = line[i];
		token[1] = line[i];
		token[2] = '\0';
		return ;
	}
	token[0] = line[i + 1];
	token[1] = '\0';
}

char	*rmv_quotes(const char *s)
{
	int		i;
	int		k;
	char	qoute;
	char	*rs;

	i = 0;
	k = 0;
	rs = malloc(ft_strlen(s) + 1);
	if (!rs)
		return (NULL);
	while (s[i])
	{
		if (s[i] == '\'' || s[i] == '\"')
		{
			qoute = s[i++];
			while (s[i] && s[i] != qoute)
				rs[k++] = s[i++];
			if (s[i] == qoute)
				i++;
		}
		else
			rs[k++] = s[i++];
	}
	rs[k] = '\0';
	return (rs);
}
