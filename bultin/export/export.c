/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 16:54:20 by ashaheen          #+#    #+#             */
/*   Updated: 2025/09/08 18:33:55 by ashaheen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void print_escaped_identifier(char *arg)
{
        int i;

        i = 0;
        while (arg[i])
        {
                if (arg[i] == '\\')
                        ft_putstr_fd("\\\\", 2);
                else
                        ft_putchar_fd(arg[i], 2);
                i++;
        }
}

int parse_export_arg(char *arg, char **name, char **value, int *has_eq, int *append)
{
        int i;

        i = 0;
        *name = NULL;
        *value = NULL;
        *has_eq = 0;
        *append = 0;
        while (arg[i] && arg[i] != '=')
                i++;
        if (arg[i] == '=')
        {
                *has_eq = 1;
                if (i > 0 && arg[i - 1] == '+')
                {
                        *append = 1;
                        *name = ft_substr(arg, 0, i - 1);
                }
                else
                        *name = ft_substr(arg, 0, i);
                *value = ft_strdup(arg + i + 1);
        }
        else
                *name = ft_strdup(arg);
        if (!*name || (*has_eq && !*value))
        {
                if (*name)
                        free(*name);
                if (*value)
                        free(*value);
                return (1);
        }
        return (0);
}

char *make_env_pair(char *name, char *value)
{
        char    *tmp;
        char    *pair;

        tmp = NULL;
        pair = NULL;
        if (!value)
                return (ft_strdup(name));
        tmp = ft_strjoin(name, "=");
        if (!tmp)
                return (NULL);
        pair = ft_strjoin(tmp, value);
        free(tmp);
        if (!pair)
                return (NULL);
        return (pair);
}

char    **append_env(char **envp, char *entry)
{
        char    **newv;
        int             n;
        int             i;

        n = 0;
        while (envp && envp[n])
                n++;
        newv = (char **)malloc(sizeof(char *) * (n + 2));
        if (!newv)
                return (NULL);
        i = 0;
        while (i < n)
        {
                newv[i] = envp[i];
                i++;
        }
        newv[n] = entry;
        newv[n + 1] = NULL;
        free(envp);
        return (newv);
}

int     env_set(char ***penvp, char *name, char *value)
{
        int             idx;
        char    *pair;
        char    **nv;

        if (!penvp || !*penvp || !name)
                return (1);
        pair = make_env_pair(name, value);
        if (!pair)
                return (1);
        idx = env_index_of(*penvp, name);
        if (idx != -1)
        {
                free((*penvp)[idx]);
                (*penvp)[idx] = pair;
                return (0);
        }
        nv = append_env(*penvp, pair);
        if (!nv)
        {
                free(pair);
                return (1);
        }
        *penvp = nv;
        return (0);
}

int     env_append(char ***penvp, char *name, char *value)
{
        int     idx;
        char    *old;
        char    *joined;

        if (!penvp || !*penvp || !name || !value)
                return (1);
        idx = env_index_of(*penvp, name);
        if (idx != -1)
        {
                old = ft_strchr((*penvp)[idx], '=');
                if (old)
                        old++;
                else
                        old = "";
                joined = ft_strjoin(old, value);
                if (!joined)
                        return (1);
                if (env_set(penvp, name, joined))
                {
                        free(joined);
                        return (1);
                }
                free(joined);
        }
        else if (env_set(penvp, name, value))
                return (1);
        return (0);
}

int export_one(t_shell *shell, char *arg)
{
        char    *name;
        char    *value;
        int             has_eq;
        int             append;

        if (parse_export_arg(arg, &name, &value, &has_eq, &append))
                return (1);
        if (!is_valid_identifier(name))
        {
                ft_putstr_fd("minishell: export: `", 2);
                print_escaped_identifier(arg);
                ft_putendl_fd("': not a valid identifier", 2);
                free(name);
                free(value);
                return (1);
        }
        if (has_eq)
        {
                if (append)
                        env_append(&shell->envp, name, value);
                else
                        env_set(&shell->envp, name, value);
                export_remove(&shell->exp, name);
        }
        else if (env_index_of(shell->envp, name) == -1
                && export_index_of(shell->exp, name) == -1)
                export_add(&shell->exp, name);
        free(name);
        free(value);
        return (0);
}
