/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 13:50:47 by maabdulr          #+#    #+#             */
/*   Updated: 2025/09/07 17:07:11 by maram            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "minishell.h"

char *get_var_value(char *var_name, t_shell *shell)
{
	char	*res;
	int		i;
	int		len;

	if (ft_strncmp(var_name, "?", 2) == 0)
	{
		res = ft_itoa(shell->exit_code);
		return (res);
	}
	len = ft_strlen(var_name);
	i = 0;
	while (shell->envp[i])
	{
		if (!ft_strncmp(shell->envp[i], var_name, len) && shell->envp[i][len] == '=')
		{
			res = ft_strdup(&shell->envp[i][len + 1]);
			return (res);
		}
		i++;
	}
	res = ft_strdup("");
	return (res);
}

char    *extract_var_name(char *s, int *len)
{
    int i;
    char    *res;

    i = 0;
    // Variable names can start with letter or underscore, then contain letters, digits, underscores
    if (ft_isalpha(s[i]) || s[i] == '_')
    {
        i++;
        while (ft_isalnum(s[i]) || s[i] == '_')
            i++;
    }
    else if (ft_isdigit(s[i]))
    {
        // For positional parameters like $1, $2, etc., only take the digit
        i++;
    }
    *len = i;
    res = ft_substr(s, 0, i);
    return(res);
}

char *handle_dollar(char *input, int *i, t_shell *shell)
{
	char	*var_name;
	char	*var_value;
	int		var_len;
	int		j;
	int		in_quotes;

	// Check if there's a backslash before the dollar sign
	if (*i > 0 && input[*i - 1] == '\\')
	{
		// Backslash escapes the dollar sign, so treat it as literal
		(*i)++;
		return (ft_strdup("$"));
	}

	// Check if we're inside quotes by looking backwards
	in_quotes = 0;
	j = *i - 1;
	while (j >= 0 && input[j] != ' ' && input[j] != '|' && input[j] != '<' && input[j] != '>')
	{
		if (input[j] == '"' || input[j] == '\'')
		{
			in_quotes = 1;
			break;
		}
		j--;
	}

	if (in_quotes)
	{
		// We're inside quotes, so $ should be literal
		(*i)++;
		return (ft_strdup("$"));
	}

	if (input[*i + 1] == '?')
	{
		var_value = ft_itoa(shell->exit_code);
		*i += 2;
		return (var_value);
	}
	if (!input[*i + 1] || !(ft_isalnum((unsigned char)input[*i + 1]) || input[*i + 1] == '_'))
	{
		(*i)++;
		return (ft_strdup("$"));
	}
	var_name = extract_var_name(&input[*i + 1], &var_len);
	var_value = get_var_value(var_name, shell);
	*i += var_len + 1;
	free(var_name);
	return (var_value);
}

char *handle_tilde(char *input, int *i, t_shell *shell)
{
	char	*home;
	char	*result;

	home = get_env_value("HOME", shell);
	if (!home || home[0] == '\0')
	{
		(*i)++;
		return (ft_strdup("~"));
	}
	if (input[*i + 1] == '\0' || input[*i + 1] == '/')
	{
		(*i)++;
		return (ft_strdup(home));
	}
	(*i)++;
	return (ft_strdup("~"));
}

char *expand_variables(char *input, t_shell *shell)
{
	char	*result;
	char	*expansion;
	int		i;

	i = 0;
	result = ft_strdup("");
	while (input[i])
	{
		if (input[i] == '\\' && input[i + 1] == '$')
		{
			// Handle escaped dollar sign - skip the backslash and treat $ as literal
			result = append_char(result, '$');
			i += 2;
		}
		else if (input[i] == '$')
		{
			expansion = handle_dollar(input, &i, shell);
			result = append_str(result, expansion);
			free(expansion);
		}
		else if (input[i] == '~')
		{
			expansion = handle_tilde(input, &i, shell);
			result = append_str(result, expansion);
			free(expansion);
		}
		else
		{
			result = append_char(result, input[i]);
			i++;
		}
	}
	return (result);
}

void expand_token_list(t_token *token, t_shell *shell)
{
    char *expanded;

    while (token)
    {
        if (token->quote != SINGLE_QUOTE)
        {
            if (token->type == CMD || token->type == ARG ||
                token->type == REDIR_IN || token->type == REDIR_OUT ||
                token->type == REDIR_APPEND || token->type == HEREDOC ||
                token->type == WORD)
            { 
				expanded = expand_variables(token->value, shell);
                free(token->value);
                token->value = expanded;
            }
        }
        token = token->next;
    }
}
