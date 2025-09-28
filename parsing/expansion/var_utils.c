/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   var_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 17:15:05 by maram             #+#    #+#             */
/*   Updated: 2025/09/24 17:18:34 by maram            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*get_var_value(char *var_name, t_shell *shell)
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
		if (!ft_strncmp(shell->envp[i],
				var_name, len) && shell->envp[i][len] == '=')
		{
			res = ft_strdup(&shell->envp[i][len + 1]);
			return (res);
		}
		i++;
	}
	res = ft_strdup("");
	return (res);
}

char	*extract_var_name(char *s, int *len)
{
	int		i;
	char	*res;

	i = 0;
	if (ft_isalpha(s[i]) || s[i] == '_')
	{
		i++;
		while (ft_isalnum(s[i]) || s[i] == '_')
			i++;
	}
	else if (ft_isdigit(s[i]))
	{
		i++;
	}
	*len = i;
	res = ft_substr(s, 0, i);
	return (res);
}

char	*handle_dollar(char *input, int *i, t_shell *shell)
{
	char	*var_name;
	char	*var_value;
	int		var_len;

	if (input[*i + 1] == '?')
	{
		var_value = ft_itoa(shell->exit_code);
		*i += 2;
		return (var_value);
	}
	if (!input[*i + 1]
		|| !(ft_isalnum((unsigned char)input[*i + 1])
			|| input[*i + 1] == '_'))
	{
		(*i)++;
		if (input[*i] == 39 || input[*i] == '"')
			return (ft_strdup(""));
		return (ft_strdup("$"));
	}
	var_name = extract_var_name(&input[*i + 1], &var_len);
	var_value = get_var_value(var_name, shell);
	*i += var_len + 1;
	free(var_name);
	return (var_value);
}

char	*handle_tilde(char *input, int *i, t_shell *shell)
{
	char	*home;

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
