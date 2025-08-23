/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 13:50:52 by maabdulr          #+#    #+#             */
/*   Updated: 2025/08/23 16:32:31 by ashaheen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char *append_str(char *str, char *suffix)
{
	char *new;

	new = ft_strjoin(str, suffix);
	free(str);
	return (new);
}

char *append_char(char *str, char c)
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


void    remove_empty_tokens(t_token **head)
{
    t_token *curr;
    t_token *prev;
    t_token *tmp;

    curr = *head;
    prev = NULL;
    while (curr)
    {
        if (curr->quote == NO_QUOTE && curr->value && curr->value[0] == '\0')
        {
            tmp = curr;
            if (prev)
                prev->next = curr->next;
            else
                *head = curr->next;
            curr = curr->next;
            free(tmp->value);
            free(tmp);
        }
        else
        {
            prev = curr;
            curr = curr->next;
        }
    }
}
