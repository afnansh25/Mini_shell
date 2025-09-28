/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 18:23:41 by maram             #+#    #+#             */
/*   Updated: 2025/09/24 18:23:43 by maram            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	get_shlvl_val(char *s)
{
	if (is_numeric_str(s))
		return (ft_atoi(s));
	return (0);
}

void	init_shlvl(char ***penvp)
{
	int		idx;
	char	*s;
	int		lvl;
	char	*num;

	if (!penvp || !*penvp)
		return ;
	idx = env_index_of(*penvp, "SHLVL");
	if (idx == -1)
	{
		env_set(penvp, "SHLVL", "1");
		return ;
	}
	s = (*penvp)[idx] + 6;
	lvl = get_shlvl_val(s);
	if (lvl < 0)
		lvl = 0;
	lvl++;
	num = ft_itoa(lvl);
	if (!num)
		return ;
	env_set(penvp, "SHLVL", num);
	free(num);
}
