/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 17:18:56 by maram             #+#    #+#             */
/*   Updated: 2025/09/24 17:20:24 by maram            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	check_first_pipe(t_token *t)
{
	if (t && t->type == PIPE)
		return (err_pipe());
	return (1);
}

int	check_bang(t_token *c)
{
	if (c->value && ft_strncmp(c->value, "!", 2) == 0)
	{
		if (c->next && c->next->value
			&& (ft_strchr(c->next->value, '<')
				|| ft_strchr(c->next->value, '>')
				|| ft_strchr(c->next->value, '|')
				|| ft_strchr(c->next->value, '&')))
			return (err_event());
		return (err_newline());
	}
	return (1);
}

int	check_double_pipe(t_token *c)
{
	if (c->type != PIPE)
		return (1);
	if (c->value && ft_strncmp(c->value, "||", 3) == 0)
	{
		if (c->next && c->next->type == PIPE)
			return (err_pipe());
		return (err_tok("||"));
	}
	if (c->next && c->next->type == PIPE)
		return (err_pipe());
	return (1);
}

int	check_redir_rules(t_token *c)
{
	if (!is_redir(c->type))
		return (1);
	if (!c->next)
		return (err_newline());
	if (c->next->type == PIPE)
		return (err_pipe());
	if (c->type == REDIR_IN && c->next->type == REDIR_OUT)
		return (err_newline());
	if (c->next->type != WORD)
		return (err_tok(c->next->value));
	return (1);
}

int	check_pipe_then_redir(t_token *c)
{
	if (c->type == PIPE && c->next && is_redir(c->next->type))
	{
		if (!c->next->next)
			return (err_newline());
		if (c->next->next->type != WORD)
			return (err_tok(c->next->next->value));
	}
	return (1);
}
