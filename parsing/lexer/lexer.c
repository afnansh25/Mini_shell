/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maabdulr <maabdulr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 17:27:35 by ashaheen          #+#    #+#             */
/*   Updated: 2025/09/27 16:53:24 by maabdulr         ###   ########.fr       */
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
	if (c == '|' && next == '|')
	{
		*len = 2;
		return (PIPE);
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
	return (*len = 0, WORD);
}

char	*get_word(char *line, int i, int *len, t_quote_type *quote)
{
	char	*raw;
	char	*word;

	*quote = NO_QUOTE;
	if (line[i] == '\'')
		*quote = SINGLE_QUOTE;
	else if (line[i] == '"')
		*quote = DOUBLE_QUOTE;
	*len = scan_complex_word_length(line, i);
	raw = ft_substr(line, i, *len);
	if (!raw)
		return (NULL);
	*quote = determine_quote_type(raw);
	word = handle_complex_quotes(raw);
	free(raw);
	return (word);
}

int	create_token(char *line, int *i, t_token **tok, int *len)
{
	t_token_type	type;
	t_quote_type	quote;
	char			*val;

	type = get_token_type(line[*i], line[*i + 1], len);
	if (type == WORD)
		val = get_word(line, *i, len, &quote);
	else
	{
		val = ft_substr(line, *i, *len);
		quote = NO_QUOTE;
	}
	if (!val)
		return (0);
	*tok = new_token(val, type, quote);
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

void	tokens(char *line, t_token **head, t_shell *shell)
{
	int	i;

	if (has_unclosed_quote(line, shell))
		return ;
	i = 0;
	while (line[i])
	{
		while (line[i] == ' ')
			i++;
		if (!line[i])
			break ;
		if (line[i] == ';')
		{
			semicolon_error(head, shell, line[i + 1]);
			return ;
		}
		if (!handle_token(head, line, &i))
			return ;
	}
	if (*head && !validate_syntax(*head))
	{
		shell->exit_code = 258;
		free_tokens(*head);
		*head = NULL;
	}
}
