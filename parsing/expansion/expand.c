/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maabdulr <maabdulr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 13:50:47 by maabdulr          #+#    #+#             */
/*   Updated: 2025/09/22 18:44:03 by maabdulr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


static void	remove_quote_markers(char *s)
{
	size_t	i;
	size_t	j;

	if (!s)
		return ;
	i = 0;
	j = 0;
	while (s[i])
	{
		if (s[i] != '\x01' && s[i] != '\x02' && s[i] != '\x03')
			s[j++] = s[i];
		i++;
	}
	s[j] = '\0';
}

int	do_skip_ctrl(t_exp *x)
{
	if (x->s[x->i] == '\x01')
	{
		x->i += 1;
		return (1);
	}
	return (0);
}

char	*expand_variables(char *input, t_shell *shell)
{
	t_exp	x;

	x.s = input;
	x.sh = shell;
	x.i = 0;
	x.f = 0;
	x.res = ft_strdup("");
	if (!x.res)
		return (NULL);
	while (x.s[x.i])
	{
		if (do_backslash(&x) || do_singlequote(&x) || do_doublequote(&x)
			|| do_dollar(&x) || do_tilde(&x) || do_skip_ctrl(&x))
			continue ;
		x.res = append_char(x.res, x.s[x.i]);
		x.i += 1;
	}
	return (x.res);
}


void	expand_token_list(t_token *token, t_shell *shell)
{
	char	*expanded;

	while (token)
	{
		if (token->quote != SINGLE_QUOTE
			&& (token->type == CMD || token->type == ARG
				|| token->type == REDIR_IN || token->type == REDIR_OUT
				|| token->type == REDIR_APPEND || token->type == HEREDOC
				|| token->type == WORD))
		{
			expanded = expand_variables(token->value, shell);
			free(token->value);
			token->value = expanded;
		}
		remove_quote_markers(token->value);
		token = token->next;
	}
}
