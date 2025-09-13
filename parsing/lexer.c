/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 17:27:35 by ashaheen          #+#    #+#             */
/*   Updated: 2025/09/10 18:32:38 by maram            ###   ########.fr       */
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
		return (PIPE); // We'll use PIPE for || for now, could add LOGICAL_OR type later
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
	word = handle_complex_quotes(raw);
	free(raw);
	return (word);
}

int	create_token(char *line, int *i, t_token **tok, int *len)
{
	t_token_type	type;
	t_quote_type quote;
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

int	validate_syntax(t_token *tokens)
{
	t_token	*current;
	int		has_cmd;

	if (!tokens)
		return (0);
	if (tokens->type == PIPE)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
		return (0);
	}
	current = tokens;
	has_cmd = 0;
	while (current)
	{
		// Check for ! character syntax error
		if (current->value && ft_strncmp(current->value, "!", 2) == 0)
		{
			// Check if ! is followed by something that could be a history expansion
			if (current->next && current->next->value && 
				(ft_strchr(current->next->value, '<') || ft_strchr(current->next->value, '>') ||
				 ft_strchr(current->next->value, '|') || ft_strchr(current->next->value, '&')))
			{
				ft_putstr_fd("minishell: !: event not found\n", 2);
				return (0);
			}
			else
			{
				ft_putstr_fd("minishell: syntax error near unexpected token `newline'\n", 2);
				return (0);
			}
		}
		    // Check for || syntax error - mimic bash behavior for ||| sequences
               if (current->value && ft_strncmp(current->value, "||", 3) == 0)
               {
                       if (current->next && current->next->type == PIPE)
                               ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
                       else
                               ft_putstr_fd("minishell: syntax error near unexpected token `||'\n", 2);
                       return (0);
               }
		if ((current->type == REDIR_IN || current->type == REDIR_OUT || 
			 current->type == REDIR_APPEND || current->type == HEREDOC) && 
			(!current->next || current->next->type != WORD))
		{
			if (!current->next)
				ft_putstr_fd("minishell: syntax error near unexpected token `newline'\n", 2);
			else
			{
				// Check for invalid redirection patterns like <>
				if (current->type == REDIR_IN && current->next && 
					current->next->type == REDIR_OUT)
				{
					ft_putstr_fd("minishell: syntax error near unexpected token `newline'\n", 2);
				}
				else
				{
					ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
					ft_putstr_fd(current->next->value, 2);
					ft_putstr_fd("'\n", 2);
				}
			}
			return (0);
		}
		if ((current->type == REDIR_IN || current->type == REDIR_OUT || 
			 current->type == REDIR_APPEND || current->type == HEREDOC) && 
			current->next && current->next->type == PIPE)
		{
			ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
			return (0);
		}
		if (current->type == PIPE && current->next && 
			(current->next->type == REDIR_IN || current->next->type == REDIR_OUT || 
			 current->next->type == REDIR_APPEND || current->next->type == HEREDOC))
		{
			if (!current->next->next || current->next->next->type != WORD)
			{
				if (!current->next->next)
					ft_putstr_fd("minishell: syntax error near unexpected token `newline'\n", 2);
				else
				{
					ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
					ft_putstr_fd(current->next->next->value, 2);
					ft_putstr_fd("'\n", 2);
				}
				return (0);
			}
		}
		if (current->type == CMD || current->type == ARG)
			has_cmd = 1;
		current = current->next;
	}
	if (tokens && has_cmd)
	{
		current = tokens;
		while (current->next)
			current = current->next;
		if (current->type == PIPE)
		{
			ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
			return (0);
		}
	}
	return (1);
}

void    tokens(char *line, t_token **head, t_shell *shell)
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
		if (line[i] == ';')
        {
            ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
            if (line[i + 1] == ';')
                ft_putstr_fd(";;", 2);
            else
                ft_putstr_fd(";", 2);
            ft_putstr_fd("'\n", 2);
            shell->exit_code = 2;
            free_tokens(*head);
            *head = NULL;
            return;
        }
        if (!handle_token(head, line, &i))
            return ;
    }
    if (*head)
    {
        if (!validate_syntax(*head))
        {
            shell->exit_code = 2;
            free_tokens(*head);
            *head = NULL;
            return;
        }
    }
}


