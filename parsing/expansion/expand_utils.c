/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 13:50:52 by maabdulr          #+#    #+#             */
/*   Updated: 2025/10/05 17:29:27 by ashaheen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*append_str(char *str, char *suffix)
{
	char	*new;

	new = ft_strjoin(str, suffix);
	free(str);
	return (new);
}

char	*append_char(char *str, char c)
{
	int		len;
	char	*new;

	len = ft_strlen(str);
	new = malloc(len + 2);
	if (!new)
		return (NULL);
	ft_memcpy(new, str, len);
	new[len] = c;
	new[len + 1] = '\0';
	free(str);
	return (new);
}

int	after_redir(t_token *prev, t_token *curr, t_shell *sh)
{
	if (!prev || !is_redir(prev->type) || !curr->value)
		return (0);
	if (ft_strchr(curr->value, ' ') || ft_strchr(curr->value, '\t'))
	{
		if (!curr->ambiguous)
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(curr->value, 2);
			ft_putendl_fd(": ambiguous redirect", 2);
			curr->ambiguous = 1;
		}
		sh->exit_code = 1;
		return (1);
	}
	return (0);
}

int	handle_empty_token(t_token **head, t_token **prev,
				t_token **curr, t_shell *sh)
{
	t_token	*tmp;

	if (!(*curr)->value || (*curr)->value[0] != '\0'
		|| (*curr)->quote != NO_QUOTE)
		return (0);
	if (*prev && is_redir((*prev)->type))
	{
		ft_putstr_fd("minishell: ambiguous redirect\n", 2);
		sh->exit_code = 1;
		return (-1);
	}
	tmp = *curr;
	if (*prev)
		(*prev)->next = (*curr)->next;
	else
		*head = (*curr)->next;
	*curr = (*curr)->next;
	free(tmp->value);
	free(tmp);
	return (1);
}

int	remove_empty_tokens(t_token **head, t_shell *sh)
{
	t_token	*c;
	t_token	*p;
	int		r;

	c = *head;
	p = NULL;
	while (c)
	{
		r = handle_empty_token(head, &p, &c, sh);
		if (r < 0)
			return (1);
		if (r > 0)
			continue ;
		if (c->quote == NO_QUOTE && c->value && after_redir(p, c, sh))
			return (1);
		p = c;
		c = c->next;
	}
	return (0);
}
