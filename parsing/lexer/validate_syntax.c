/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validate_syntax.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 17:23:46 by maram             #+#    #+#             */
/*   Updated: 2025/09/24 17:23:47 by maram            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ends_with_pipe(t_token *t)
{
	while (t && t->next)
		t = t->next;
	if (t && t->type == PIPE)
		return (err_pipe());
	return (1);
}

int	validate_syntax(t_token *tokens)
{
	t_token	*cur;
	int		has_cmd;

	if (!tokens || !check_first_pipe(tokens))
		return (0);
	cur = tokens;
	has_cmd = 0;
	while (cur)
	{
		if (!check_bang(cur) || !check_double_pipe(cur)
			|| !check_redir_rules(cur) || !check_pipe_then_redir(cur))
			return (0);
		if (cur->type == CMD || cur->type == ARG)
			has_cmd = 1;
		cur = cur->next;
	}
	if (has_cmd && !ends_with_pipe(tokens))
		return (0);
	return (1);
}
