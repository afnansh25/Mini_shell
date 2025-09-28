/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   varr_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 17:18:38 by maram             #+#    #+#             */
/*   Updated: 2025/09/24 17:18:43 by maram            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_dq_escape(char c)
{
	return (c == '"' || c == '$' || c == '\\' || c == '`');
}

int	handle_dquote_escape(t_exp *x)
{
	if (!x->s[x->i + 1])
	{
		x->res = append_char(x->res, '\\');
		x->i += 1;
		return (1);
	}
	if (is_dq_escape(x->s[x->i + 1]))
	{
		x->res = append_char(x->res, x->s[x->i + 1]);
		x->i += 2;
		return (1);
	}
	x->res = append_char(x->res, '\\');
	x->i += 1;
	return (1);
}

int	handle_default_escape(t_exp *x)
{
	if (x->s[x->i + 1] == '$')
	{
		x->res = append_char(x->res, '$');
		x->i += 2;
		return (1);
	}
	x->res = append_char(x->res, '\\');
	x->i += 1;
	return (1);
}

int	do_backslash(t_exp *x)
{
	if (x->s[x->i] != '\\' || (x->f & 1))
		return (0);
	if (x->f & 2)
		return (handle_dquote_escape(x));
	return (handle_default_escape(x));
}
