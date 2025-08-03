//header
#include "minishell.h"

int g_exit_code;

char *get_var_value(char *var_name, char **envp)
{
	char	*res;
	int		i;
	int		len;

	if (ft_strncmp(var_name, "?", 2) == 0)
	{
		res = ft_itoa(g_exit_code);
		return (res);
	}
	len = ft_strlen(var_name);
	i = 0;
	while (envp[i])
	{
		if (!ft_strncmp(envp[i], var_name, len) && envp[i][len] == '=')
		{
			res = ft_strdup(&envp[i][len + 1]);
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
    while (ft_isalnum(s[i]) || s[i] == '_')
        i++;
    *len = i;
    res = ft_substr(s, 0, i);
    return(res);
}

char *handle_dollar(char *input, int *i, char **envp)
{
	char	*var_name;
	char	*var_value;
	int		var_len;

	if (input[*i + 1] == '?')
	{
		var_value = ft_itoa(g_exit_code);
		*i += 2;
		return (var_value);
	}
	var_name = extract_var_name(&input[*i + 1], &var_len);
	var_value = get_var_value(var_name, envp);
	*i += var_len + 1;
	free(var_name);
	return (var_value);
}

char *expand_variables(char *input, char **envp)
{
	char	*result;
	char	*expansion;
	int		i;

	i = 0;
	result = ft_strdup("");
	while (input[i])
	{
		if (input[i] == '$')
		{
			expansion = handle_dollar(input, &i, envp);
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

void expand_token_list(t_token *token, char **envp)
{
    char *expanded;

    while (token)
    {
        if (token->quote != SINGLE_QUOTE)
        {
            if (token->type == CMD || token->type == ARG ||
                token->type == REDIR_IN || token->type == REDIR_OUT ||
                token->type == REDIR_APPEND || token->type == HEREDOC)
            {
                expanded = expand_variables(token->value, envp);
                free(token->value);
                token->value = expanded;
            }
        }
        token = token->next;
    }
}
