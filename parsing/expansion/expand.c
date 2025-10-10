/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 13:50:47 by maabdulr          #+#    #+#             */
/*   Updated: 2025/10/08 15:47:35 by maram            ###   ########.fr       */
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

static void	expand_one_token(t_token *tok, t_token *prev, t_shell *sh)
{
	char	*old;
	char	*new;

	old = tok->value;
	new = expand_variables(tok->value, sh);
	if (!new)
	{
		free(old);
		tok->value = NULL;
		return ;
	}
	if (prev && is_redir(prev->type)
		&& tok->quote == NO_QUOTE && new[0] == '\0')
	{
		tok->ambiguous = 1;
		sh->exit_code = 1;
		free(new);
		tok->value = old;
		return ;
	}
	free(old);
	tok->value = new;
}

void	expand_token_list(t_token *tok, t_shell *sh)
{
	t_token	*prev;
	int		is_lim;

	prev = NULL;
	while (tok)
	{
		is_lim = (prev && prev->type == HEREDOC);
		if (!is_lim && tok->quote != SINGLE_QUOTE
			&& (tok->type == CMD || tok->type == ARG
				|| tok->type == REDIR_IN || tok->type == REDIR_OUT
				|| tok->type == REDIR_APPEND || tok->type == HEREDOC
				|| tok->type == WORD))
			expand_one_token(tok, prev, sh);
		if (tok->value)
			remove_quote_markers(tok->value);
		prev = tok;
		tok = tok->next;
	}
}
