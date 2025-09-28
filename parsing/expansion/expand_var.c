/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_var.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 17:14:50 by maram             #+#    #+#             */
/*   Updated: 2025/09/24 17:14:51 by maram            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	do_singlequote(t_exp *x)
{
	if (x->s[x->i] == '\x02')
	{
		x->f ^= 1;
		x->i += 1;
		return (1);
	}
	return (0);
}

int	do_doublequote(t_exp *x)
{
	if (x->s[x->i] == '\x03')
	{
		x->f ^= 2;
		x->i += 1;
		return (1);
	}
	return (0);
}

int	do_dollar(t_exp *x)
{
	char	*tmp;

	if (x->s[x->i] == '$' && !(x->f & 1))
	{
		tmp = handle_dollar(x->s, &x->i, x->sh);
		x->res = append_str(x->res, tmp);
		free(tmp);
		return (1);
	}
	return (0);
}

int	do_tilde(t_exp *x)
{
	char	*tmp;

	if (x->s[x->i] == '~' && !(x->f & 3))
	{
		tmp = handle_tilde(x->s, &x->i, x->sh);
		x->res = append_str(x->res, tmp);
		free(tmp);
		return (1);
	}
	return (0);
}
