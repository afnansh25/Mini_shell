/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 14:39:59 by ashaheen          #+#    #+#             */
/*   Updated: 2025/08/13 17:40:28 by ashaheen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int   export_index_of(char **exp, char *name)
{
    int i;

    if (!exp || !name)
        return (-1);
    i = 0;
    while (exp[i])
    {
        if (ft_strncmp(exp[i], name, ft_strlen(name) + 1) == 0)
            return (i);
        i++;
    }
    return (-1);
}

int   export_add(char ***pexp, char *name)
{
    int     n;
    int     i;
    char    **newv;

    if (!name)
        return (1);
    if (export_index_of(*pexp, name) != -1)
        return (0);
    n = 0;
    if (*pexp)
        while ((*pexp)[n])
            n++;
    newv = malloc(sizeof(char *) * (n + 2));
    if (!newv)
        return (1);
    i = 0;
    while(i++ < n)
        newv[i] = (*pexp)[i];
    newv[n] = ft_strdup(name);
    if (!newv[n])
    {
        free(newv);
        return (1);
    }
    newv[n + 1] = NULL;
    free(*pexp);
    *pexp = newv;
    return (0);
}

void   export_remove(char ***pexp, char *name)
{
    int idx;
    int i;

    if (!pexp || !*pexp)
        return ;
    idx = export_index_of(*pexp, name);
    if (idx == -1)
        return ;
    free((*pexp)[idx]);
    i = idx;
    while ((*pexp)[i])
    {
        (*pexp)[i] = (*pexp)[i + 1];
        i++;
    }
}