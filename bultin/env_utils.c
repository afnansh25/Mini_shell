/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maabdulr <maabdulr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 14:49:26 by codespace         #+#    #+#             */
/*   Updated: 2025/09/22 19:25:51 by maabdulr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

long long	ft_atoll(const char *s)
{
	int         i;
	int         sign;
	long long   n;

	i = 0;
	sign = 1;
	n = 0;
	if (s[i] == '+' || s[i] == '-')
	{
		if (s[i] == '-')
			sign = -1;
		i++;
	}
	while (s[i] && s[i] >= '0' && s[i] <= '9')
	{
		n = n * 10 + (s[i] - '0');
		i++;
	}
	return (n * sign);
}

int  env_count(char **env)
{
    int i;

    i = 0;
    while (env && env[i])
        i++;
    return (i);
}

void    free_envp(char **env)
{
    int i;

    if (!env)
        return;
    i = 0;
    while (env[i])
    {
        free(env[i]);
        i++;
    }
    free(env);
}

void init_shlvl(char ***penvp)
{
	int     idx;
	char    *s;
	int     lvl;
	char    *num;

	if (!penvp || !*penvp)
		return ;
	idx = env_index_of(*penvp, "SHLVL");
	if (idx == -1)
		return ((void)env_set(penvp, "SHLVL", "1"));
	s = (*penvp)[idx] + 6;
	lvl = (is_numeric_str(s) ? ft_atoi(s) : 0);
	if (lvl < 0) lvl = 0;
	lvl += 1;
	num = ft_itoa(lvl);
	if (!num) return ;
	env_set(penvp, "SHLVL", num);
	free(num);
}
void init_uid(char ***penvp)
{
    uid_t   uid;
    char    *uid_str;
    int     idx;

    if (!penvp || !*penvp)
        return ;
    idx = env_index_of(*penvp, "UID");
    if (idx == -1)
        return ;
    uid = getuid();
    uid_str = ft_itoa(uid);
    if (!uid_str)
        return ;
    env_set(penvp, "UID", uid_str);
    free(uid_str);
}
