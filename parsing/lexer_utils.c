/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 11:25:15 by ashaheen          #+#    #+#             */
/*   Updated: 2025/09/09 20:53:40 by maram            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


// char token like "echo", ">"
t_token	*new_token(char *token, t_token_type type, t_quote_type quote)
{
	t_token	*new_token;

	new_token = malloc(sizeof(t_token));
	if (!new_token)
		return (NULL);
	new_token->value = token;
    new_token->type = type;
	new_token->quote = quote;
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

void	print_syntax_error(char *line, int i)
{
	char            token[3];

	if (!line[i + 1] || line[i + 1] == '\0'
			|| (line[i] == '<' && line[i + 1] == '>'
					&& (!line[i + 2] || line[i + 2] == '\0'))
			|| (line[i] == '<' && line[i + 1] == '<'
					&& line[i + 2] == '<'
					&& (!line[i + 3] || line[i + 3] == '\0'))
			|| (line[i] == '>' && line[i + 1] == '>'
					&& line[i + 2] == '>'
					&& (!line[i + 3] || line[i + 3] == '\0')))
	{
			ft_putstr_fd(
					"minishell: syntax error near unexpected token `newline'\n",
					2);
			return ;
	}
	if ((line[i] == '|' && line[i + 1] == '|')
			|| (line[i] == '<' && line[i + 1] == '<'
					&& line[i + 2] == '<')
			|| (line[i] == '>' && line[i + 1] == '>'
					&& line[i + 2] == '>'))
	{
			token[0] = line[i];
			token[1] = '\0';
	}
	else if (line[i + 1] == line[i])
	{
			token[0] = line[i];
			token[1] = line[i];
			token[2] = '\0';
	}
	else
	{
			token[0] = line[i + 1];
			token[1] = '\0';
	}
	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
	ft_putstr_fd(token, 2);
	ft_putstr_fd("'\n", 2);
}

int	is_invalid_sequence(char *line, int i)
{
	if (!line[i])
		return (0);
	// <<< or <<<<
	if (line[i] == '<' && line[i + 1] == '<' && line[i + 2] == '<')
		return (1);
	// >>> or >>>>
	if (line[i] == '>' && line[i + 1] == '>' && line[i + 2] == '>')
		return (1);
	// ||
	if (line[i] == '|' && line[i + 1] == '|')
		return (1);
	// <| |< >| |>
	if ((line[i] == '<' && line[i + 1] == '|') ||
		(line[i] == '|' && line[i + 1] == '<') ||
		(line[i] == '>' && line[i + 1] == '|') ||
		(line[i] == '|' && line[i + 1] == '>'))
		return (1);
	// >< or <>
	if ((line[i] == '>' && line[i + 1] == '<') ||
		(line[i] == '<' && line[i + 1] == '>'))
		return (1);
	return (0);
}

int scan_word_length(char *line, int i)
{
	int j = 0;
	char q;

	while (line[i + j] && line[i + j] != ' ' &&
		line[i + j] != '|' && line[i + j] != '<' && line[i + j] != '>')
	{
		if (line[i + j] == '\'' || line[i + j] == '\"')
		{
			q = line[i + j++];
			while (line[i + j] && line[i + j] != q)
				j++;
			if (line[i + j] == q)
				j++;
		}
		else
			j++;
	}
	return (j);
}

int scan_complex_word_length(char *line, int i)
{
	int j = 0;
	char q;

	while (line[i + j] && line[i + j] != ' ' &&
		line[i + j] != '|' && line[i + j] != '<' && line[i + j] != '>')
	{
		if (line[i + j] == '$' && (line[i + j + 1] == '\'' || line[i + j + 1] == '"'))
		{
			// Handle $'...' or $"..." patterns
			q = line[i + j + 1];
			j += 2; // Skip $ and opening quote
			while (line[i + j] && line[i + j] != q)
				j++;
			if (line[i + j] == q)
				j++; // Skip closing quote
		}
		else if (line[i + j] == '\'' || line[i + j] == '"')
		{
				q = line[i + j++];
				while (line[i + j] && line[i + j] != q)
						j++;
				if (line[i + j] == q)
						j++;
		}
		else
				j++;
}
return (j);
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

char *handle_complex_quotes(const char *s)
{
	int i;
	int k;
	char quote;
	char *rs;

	i = 0;
	k = 0;
	rs = malloc(ft_strlen(s) + 1);
	if (!rs)
		return (NULL);
	while (s[i])
	{
		if (s[i] == '$' && (s[i + 1] == '\'' || s[i + 1] == '"'))
                {
                        // Handle $'...' or $"..." patterns
                        quote = s[i + 1];
                        i += 2; // Skip $ and opening quote
                        while (s[i] && s[i] != quote)
                        {
                                rs[k++] = s[i++];
                        }
                        if (s[i] == quote)
                                i++; // Skip closing quote
                        if (s[i])
                                rs[k++] = '\x01';
                }
                else if (s[i] == '\'' || s[i] == '"')
                {
                        quote = s[i++];
                        while (s[i] && s[i] != quote)
                        {
                                rs[k++] = s[i++];
                        }
                        if (s[i] == quote)
                                i++; // Skip closing quote
                        if (s[i])
				rs[k++] = '\x01';
                }
                  
		else if (s[i] == '\\')
		{
			// Handle backslash escaping
			if (s[i + 1] == '$')
			{
				// Preserve backslash before dollar sign for variable expansion
				rs[k++] = s[i++];
				rs[k++] = s[i++];
			}
			else if (s[i + 1])
			{
				rs[k++] = s[i + 1];
				i += 2;
			}
			else
			{
				rs[k++] = s[i++];
			}
		}
		else
		{
			rs[k++] = s[i++];
		}
	}
	rs[k] = '\0';
	return (rs);
}

