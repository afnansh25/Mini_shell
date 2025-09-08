/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 13:50:52 by maabdulr          #+#    #+#             */
/*   Updated: 2025/09/08 18:43:41 by ashaheen         ###   ########.fr       */
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

static int  is_redir(t_token_type type)
{
    return (type == REDIR_IN || type == REDIR_OUT
        || type == REDIR_APPEND || type == HEREDOC);
}

int remove_empty_tokens(t_token **head, t_shell *shell)
{
    t_token *curr;
    t_token *prev;
    t_token *tmp;

    curr = *head;
    prev = NULL;
    while (curr)
    {
        if (curr->quote == NO_QUOTE && curr->value)
        {
            if (curr->value[0] == '\0')
            {
                if (prev && is_redir(prev->type))
                {
                    ft_putstr_fd("minishell: ambiguous redirect\n", 2);
                    shell->exit_code = 1;
                    return (1);
                }
                tmp = curr;
                if (prev)
                    prev->next = curr->next;
                else
                    *head = curr->next;
                curr = curr->next;
                free(tmp->value);
                free(tmp);
                continue ;
            }
            if (prev && is_redir(prev->type)
                && (ft_strchr(curr->value, ' ') || ft_strchr(curr->value, '\t')))
            {
                ft_putstr_fd("minishell: ", 2);
                ft_putstr_fd(curr->value, 2);
                ft_putendl_fd(": ambiguous redirect", 2);
                shell->exit_code = 1;
                return (1);
            }
        }
        prev = curr;
        curr = curr->next;
    }
    return (0);
}

static int  is_assignment_word(const char *s)
{
    int i;

    if (!s || !(ft_isalpha((unsigned char)s[0]) || s[0] == '_'))
        return (0);
    i = 1;
    while (s[i] && s[i] != '=')
    {
        if (!(ft_isalnum((unsigned char)s[i]) || s[i] == '_'))
            return (0);
        i++;
    }
    if (s[i] != '=')
        return (0);
    return (1);
}

static char **split_whitespace(char *str)
{
    char    *tmp;
    char    **res;
    int             i;

    tmp = ft_strdup(str);
    if (!tmp)
        return (NULL);
    i = 0;
    while (tmp[i])
    {
        if (tmp[i] == '\t')
            tmp[i] = ' ';
        i++;
    }
    res = ft_split(tmp, ' ');
    free(tmp);
    return (res);
}

void split_token_list(t_token **head)
{
    t_token *curr;
    char    **parts;
    int             i;
    t_token *new;

    curr = *head;
    while (curr)
    {
        if (curr->quote == NO_QUOTE && curr->value
            && (ft_strchr(curr->value, ' ') || ft_strchr(curr->value, '\t'))
            && (curr->type == CMD || curr->type == ARG || curr->type == WORD)
            && !ft_strchr(curr->value, '='))
        {
            parts = split_whitespace(curr->value);
            if (parts && parts[0])
            {
                free(curr->value);
                curr->value = ft_strdup(parts[0]);
                i = 1;
                while (parts[i])
                {
                    new = malloc(sizeof(t_token));
                    if (!new)
                        break;
                    new->value = ft_strdup(parts[i]);
                    new->type = ARG;
                    new->quote = NO_QUOTE;
                    new->next = curr->next;
                    curr->next = new;
                    curr = new;
                    i++;
                }
                free_arr(parts);
                curr = curr->next;
                continue ;
            }
            if (parts)
                free_arr(parts);
        }
        curr = curr->next;
    }
}