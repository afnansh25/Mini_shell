/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maabdulr <maabdulr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 17:27:35 by ashaheen          #+#    #+#             */
/*   Updated: 2025/07/26 12:24:36 by maabdulr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// char token like "echo", ">"
t_token	*new_token(char *token, t_token_type type)
{
	t_token	*new_token;

	new_token = malloc(sizeof(t_token));
	if (!new_token)
		return (NULL);
	new_token->value = token;
    new_token->type = type;
	new_token->next = NULL;
	return (new_token);
}

// t_token *node;
// node = new_token("ls", WORD);
// [node] 
//   ├ value = "ls"
//   ├ type  = WORD  it mean (0)
//   └ next  = NULL

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

char *rmv_quotes(const char *s)
{
	int i;
	int k ;
	char qoute;
	char *rs;

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
			while(s[i] && s[i] != qoute)
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
        {
            j++;
        }
    }
    *len = j;
    raw = ft_substr(line, i, j);
    if (!raw)
        return NULL;

    word = rmv_quotes(raw);
    free(raw);
    return word;
}

void tokens(char *line, t_token **head)
{
    t_token      *tok;
    t_token_type  type;
    char         *val;
    int           i;
	int len;

	i = 0;
    while (line[i])
    {
        if (line[i] == ' ')
        {
            i++;
            continue;
        }
        type = get_token_type(line[i], line[i + 1], &len);
        if (type == WORD)
            val = get_word(line, i, &len);
        else
            val = ft_substr(line, i, len);

        tok = new_token(val, type);
        token_add_back(head, tok);
        i += len;
    }
}
