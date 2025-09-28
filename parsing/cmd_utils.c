/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 17:52:05 by maram             #+#    #+#             */
/*   Updated: 2025/09/24 17:52:15 by maram            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	copy_argv_to_list(t_arg **list, char **argv)
{
	int	i;

	i = 0;
	while (argv && argv[i])
	{
		if (!add_arg(list, argv[i]))
			return (0);
		i++;
	}
	return (1);
}

int	rebuild_argv_from_list(t_cmd *cmd, t_arg *list)
{
	free_arr(cmd->argv);
	cmd->argv = arg_list_to_array(list);
	return (cmd->argv != NULL);
}

int	add_arg_to_cmd(t_cmd *cmd, char *val)
{
	t_arg	*list;

	list = NULL;
	if (!copy_argv_to_list(&list, cmd->argv))
	{
		free_arg_list(list);
		return (0);
	}
	if (!add_arg(&list, val))
	{
		free_arg_list(list);
		return (0);
	}
	if (!rebuild_argv_from_list(cmd, list))
	{
		free_arg_list(list);
		return (0);
	}
	return (1);
}
