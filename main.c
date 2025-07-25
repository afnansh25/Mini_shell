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

void	print_tokens(t_token *t)
{
	while (t)
	{
		printf("[%d] \"%s\"\n", t->type, t->value);
		t = t->next;
	}
}
void	free_tokens(t_token *token)
{
	t_token	*tmp;

	while (token)
	{
		tmp = token->next;
		free(token->value);
		free(token);
		token = tmp;
	}
}

int main(void)
{
    char *line;
    t_token	*token_list;

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
        {
			token_list = NULL;
			tokens(line, &token_list); // 👈 CALL THE LEXER HERE
			print_tokens(token_list);  // 👈 optional: debug print function
			free_tokens(token_list);   // 👈 optional: clean up
		}
        add_history(line);
        free(line);
    }
    return (0);
}