/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_args.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 17:48:34 by maram             #+#    #+#             */
/*   Updated: 2025/09/24 17:51:59 by maram            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	add_arg(t_arg **list, char *val)
{
	t_arg	*new;
	t_arg	*last;

	new = malloc(sizeof(t_arg));
	if (!new)
		return (0);
	new->value = ft_strdup(val);
	if (!new->value)
	{
		free(new);
		return (0);
	}
	new->next = NULL;
	if (!*list)
		*list = new;
	else
	{
		last = *list;
		while (last->next)
			last = last->next;
		last->next = new;
	}
	return (1);
}

char	**arg_list_to_array(t_arg *list)
{
	char	**arr;
	t_arg	*tmp;
	int		count;
	int		i;

	tmp = list;
	count = 0;
	while (tmp)
	{
		count++;
		tmp = tmp->next;
	}
	arr = malloc(sizeof(char *) * (count + 1));
	if (!arr)
		return (NULL);
	i = 0;
	while (list && i < count)
	{
		arr[i++] = list->value;
		tmp = list;
		list = list->next;
		free(tmp);
	}
	arr[i] = NULL;
	return (arr);
}

void	free_arg_list(t_arg *list)
{
	t_arg	*tmp;

	while (list)
	{
		tmp = list->next;
		free(list->value);
		free(list);
		list = tmp;
	}
}

t_arg	*collect_args(t_token **token_ptr)
{
	t_arg	*list;

	list = NULL;
	if (*token_ptr && (*token_ptr)->type == CMD)
	{
		if (!add_arg(&list, (*token_ptr)->value))
			return (NULL);
		*token_ptr = (*token_ptr)->next;
	}
	while (*token_ptr && (*token_ptr)->type == ARG)
	{
		if (!add_arg(&list, (*token_ptr)->value))
		{
			free_arg_list(list);
			return (NULL);
		}
		*token_ptr = (*token_ptr)->next;
	}
	return (list);
}

void	handle_cmd_and_args(t_cmd *cmd, t_token **token_ptr)
{
	t_arg	*list;

	list = collect_args(token_ptr);
	if (!list)
	{
		cmd->argv = NULL;
		return ;
	}
	cmd->argv = arg_list_to_array(list);
	if (!cmd->argv)
		cmd->argv = NULL;
}
