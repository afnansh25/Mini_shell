/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 17:27:35 by ashaheen          #+#    #+#             */
/*   Updated: 2025/07/23 17:27:53 by ashaheen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

size_t ft_strlen(const char *s)
{
    size_t i;
    
    i = 0;
    while (s[i])
        i++;
    return (i);
}

char    *ft_substr(char const *s, unsigned int start, size_t len)
{
    char    *sstr;
    size_t i;
    
    i = 0;
    if (!s)
        return (NULL);
    if (ft_strlen(s) < start)
        len = 0;
    if (ft_strlen(s) - start < len)
        len = ft_strlen(s) - start;
    sstr = (char *)malloc(sizeof (char) * len + 1);
    if(!sstr)
        return (NULL);
    while (i < len)
    {
        sstr[i++] = s[start++];
    }
    sstr[i] = '\0';
    return (char *)sstr; 
}

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

char	*get_word(char *line, int i, int *len)
{
	int		j;
	char	quote;
    char    *word;

	j = 0;
	if (line[i] == '\'' || line[i] == '\"')
	{
		quote = line[i];
		j++;
		while (line[i + j] && line[i + j] != quote)
			j++;
		if (line[i + j] == quote)
			j++;
	}
	else
	{
		while (line[i + j] && line[i + j] != ' ' && line[i + j] != '|'
			&& line[i + j] != '<' && line[i + j] != '>' && line[i + j] != '\'' && line[i + j] != '\"')
			j++;
	}
	*len = j;
    word = ft_substr(line, i, j);
	return (word);
}

void	tokens(char *line, t_token **head)
{
	t_token		    *token;
	t_token_type    type;
	char		    *value;
	int			    i;
	int			    len;

	i = 0;
	while (line[i])
	{
		if (line[i] == ' ')
		{
			i++;
			continue ;
		}
		type = get_token_type(line[i], line[i + 1], &len);
		if (type == WORD)
			value = get_word(line, i, &len);
		else
			value = ft_substr(line, i, len);
		token = new_token(value, type);
		token_add_back(head, token);
		i += len;
	}
}


// void    tokens(char *line)
// {
//     t_token *token;
//     t_token *head;
//     char    *value;
//     int     token_len;
//     int     i;

//     i = 0;
//     token = NULL;
//     head = NULL;
//     while (line[i])
//     {
//         token_len = 1;
//         if (line[i] == '<' && line[i + 1] == '<')
//         {
//             token_len = 2;
//             value = ft_substr(line, i, token_len);
//             token = new_token(value, HEREDOC);
//             i++;
//         }
//         if (line[i] == '>' && line[i + 1] == '>')
//         {
//             token_len = 2;
//             value = ft_substr(line, i, token_len);
//             token = new_token(value, REDIR_APPEND);
//             i++;
//         }
//         if (line[i] == '|')
//         {
//             value = ft_substr(line, i, token_len);
//             token = new_token(value, PIPE);
//         }
//         if (line[i] == '<')
//         {
//             value = ft_substr(line, i, token_len);
//             token = new_token(value, REDIR_IN);
//         }
//         if (line[i] == '>')
//         {
//             value = ft_substr(line, i, token_len);
//             token = new_token(value, REDIR_OUT);
//         }
//         if (token)
//             token_add_back(&head, token);
//         i += token_len;
//     }
// }
