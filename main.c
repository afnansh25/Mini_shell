/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 14:29:10 by ashaheen          #+#    #+#             */
/*   Updated: 2025/07/23 17:19:55 by ashaheen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int main(void)
{
    char *line;

    while(1)
    {
        setup_signals(); 
        line = readline("pink_bow🎀$ ");
        if (!line)
        {
            printf("exit\n");
            break;
        }
        if (*line)
            add_history(line);
        free(line);
    }
    return (0);
}