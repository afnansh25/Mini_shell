/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 17:27:35 by ashaheen          #+#    #+#             */
/*   Updated: 2025/08/02 17:40:16 by ashaheen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token_type	get_token_type(char c, char next, int *len)
{
	if (c == '<' && next == '<')
	{
		*len = 2;
		return (HEREDOC);
	}
	if (c == '>' && next == '>')
	{
		*len = 2;
		return (REDIR_APPEND);
	}
	if (c == '<' || c == '>' || c == '|')
	{
		*len = 1;
		if (c == '<')
			return (REDIR_IN);
		if (c == '>')
			return (REDIR_OUT);
		return (PIPE);
	}
	*len = 0;
	return (WORD);
}

char *get_word(char *line, int i, int *len)
{
    int  j = 0;
    char *raw;
    char *word;

    while (line[i + j] && line[i + j] != ' ' &&
           line[i + j] != '|' && line[i + j] != '<' && line[i + j] != '>')
    {
        if (line[i + j] == '\'' || line[i + j] == '\"')
        {
            char q = line[i + j++];
            while (line[i + j] && line[i + j] != q)
                j++;
            if (line[i + j] == q)
                j++;
        }
        else
            j++;
    }
    *len = j;
    raw = ft_substr(line, i, j);
    if (!raw)
        return NULL;
    word = rmv_quotes(raw);
    free(raw);
    return word;
}

int	create_token(char *line, int *i, t_token **tok, int *len)
{
	t_token_type	type;
	char			*val;

	type = get_token_type(line[*i], line[*i + 1], len);
	if (type == WORD)
		val = get_word(line, *i, len);
	else
		val = ft_substr(line, *i, *len);
	if (!val)
		return (0);
	*tok = new_token(val, type);
	if (!*tok)
	{
		free(val);
		return (0);
	}
	return (1);
}

int	handle_token(t_token **head, char *line, int *i)
{
	t_token	*tok;
	int		len;
	int		success;

	success = create_token(line, i, &tok, &len);
	if (!success)
	{
		free_tokens(*head);
		*head = NULL;
		return (0);
	}
	token_add_back(head, tok);
	*i += len;
	return (1);
}

void	tokens(char *line, t_token **head)
{
	int	i;

	i = 0;
	while (line[i])
	{
		if (line[i] == ' ')
		{
			i++;
			continue;
		}
		if (is_invalid_sequence(line, i))
		{
			print_syntax_error(line, i);
			*head = NULL;
			return ;
		}
		if (!handle_token(head, line, &i))
			return ;
	}
}

