/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 17:23:27 by maram             #+#    #+#             */
/*   Updated: 2025/09/24 17:23:40 by maram            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_stop(char c)
{
	return (c == ' ' || c == '|' || c == '<' || c == '>' || c == '\0');
}

void	skip_quoted(char *line, int i, int *j, char q)
{
	(*j)++;
	while (line[i + *j] && line[i + *j] != q)
		(*j)++;
	if (line[i + *j] == q)
		(*j)++;
}

void	skip_dollar_quote(char *line, int i, int *j)
{
	char	q;

	q = line[i + *j + 1];
	*j += 2;
	while (line[i + *j] && line[i + *j] != q)
		(*j)++;
	if (line[i + *j] == q)
		(*j)++;
}

int	scan_complex_word_length(char *line, int i)
{
	int	j;

	j = 0;
	while (line[i + j] && !is_stop(line[i + j]))
	{
		if (line[i + j] == '$'
			&& (line[i + j + 1] == '\'' || line[i + j + 1] == '"'))
			skip_dollar_quote(line, i, &j);
		else if (line[i + j] == '\'' || line[i + j] == '"')
			skip_quoted(line, i, &j, line[i + j]);
		else
			j++;
	}
	return (j);
}
