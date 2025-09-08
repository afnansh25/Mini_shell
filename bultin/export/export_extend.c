/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_extend.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 14:40:08 by ashaheen          #+#    #+#             */
/*   Updated: 2025/09/08 16:43:37 by ashaheen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void    print_escaped_value_fd(int fd, const char *s)
{
	int	i;

	if (!s)
		return ;
	i = 0;
	while (s[i])
	{
		if (s[i] == '"' || s[i] == '\\' || s[i] == '$')
				ft_putchar_fd('\\', fd);
		ft_putchar_fd(s[i], fd);
		i++;
	}	
}

// Compare "NAME" parts of "NAME=VALUE" strings
int cmp_env_names(char *a, char *b)
{
	int i;

	i = 0;
	while (a[i] && a[i] != '=' && b[i] && b[i] != '=')
	{
        if ((unsigned char)a[i] != (unsigned char)b[i])
            return ((unsigned char)a[i] - (unsigned char)b[i]);
		i++;
	}
	if (a[i] == '=' || a[i] == '\0')
	{
		if (b[i] == '=' || b[i] == '\0')
				return (0);
		return (-1);
	}
	if (b[i] == '=' || b[i] == '\0')
		return (1);
	return (0);
}

void    sort_env_ptrs(char **a)
{
        int     i;
        int     j;
        char    *tmp;

        if (!a)
                return ;
        i = 0;
        while (a[i])
        {
                j = i + 1;
                while (a[j])
                {
                        if (cmp_env_names(a[i], a[j]) > 0)
                        {
                                tmp = a[i];
                                a[i] = a[j];
                                a[j] = tmp;
                        }
                        j++;
                }
                i++;
        }
}

char	**join_env_and_exp(char **envp, char **exp)
{
	char	**copy;
	int		n;
	int		m;
	int		i;

	n = 0;
	while (envp && envp[n])
		n++;
	m = 0;
	while (exp && exp[m])
		m++;
	copy = (char **)malloc(sizeof(char *) * (n + m + 1));
	if (!copy)
		return (NULL);
	i = -1;
	while (++i < n)
		copy[i] = envp[i];
	i = -1;
	while (++i < m)
		copy[n + i] = exp[i];
	copy[n + m] = NULL;
	return (copy);
}

/* prints: declare -x KEY[="escaped value"] */

//new
void	print_decl(char *e)
{
	int	j;

	ft_putstr_fd("declare -x ", 1);
	j = 0;
	while (e[j] && e[j] != '=')
		ft_putchar_fd(e[j++], 1);
	if (e[j] == '=')
	{
		ft_putstr_fd("=\"", 1);
		print_escaped_value_fd(1, e + j + 1);
		ft_putendl_fd("\"", 1);
	}
	else
		ft_putchar_fd('\n', 1);
}

// void	export_print(char **envp, char **exp)
// {
// 	char	**copy;
// 	int		i;

// 	copy = join_env_and_exp(envp, exp);
// 	if (!copy)
// 		return ;
// 	sort_env_ptrs(copy);
// 	i = 0;
// 	while (copy[i])
// 	{
// 		print_decl(copy[i]);
// 		i++;
// 	}
// 	free(copy);
// }
