/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 20:45:48 by maram             #+#    #+#             */
/*   Updated: 2025/09/14 18:12:30 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int check_n_flag(char *arg)
{
 int j;

 if (!arg || arg[0] != '-')
  return (0);
 j = 1;
 while (arg[j] == 'n')
  j++;
 return (arg[j] == '\0' && j >= 1);
}

int exec_echo(char **av)
{
 int i;
 int newline;

 i = 1;
 newline = 1;
 while (av[i] && check_n_flag(av[i]))
 {
  newline = 0;
  i++;
 }
 while (av[i])
 {
  write(1, av[i], ft_strlen(av[i]));
  if (av[i + 1])
   write(1, " ", 1);
  i++;
 }
 if (newline)
  write(1, "\n", 1);
 return (0);
}
