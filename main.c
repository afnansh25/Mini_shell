/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maabdulr <maabdulr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 14:29:10 by ashaheen          #+#    #+#             */
/*   Updated: 2025/08/09 14:55:18 by maabdulr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int main(int ac, char **av, char **envp)
{
    char 	*line;
    t_shell	shell;
    t_token	*token_list;
	t_cmd	*cmd_list;
    (void)av;
    
    if (ac > 1)
    {
        printf("You Should Write ./minishell ");
        return(0);
    }
    shell.exit_code = 0;
	shell.envp = envp;
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
			cmd_list = NULL;
			tokens(line, &token_list); //  CALL THE LEXER HERE
			//print_tokens(token_list);  // optional: debug print function
            set_token_types(token_list);
            expand_token_list(token_list, &shell);
            //debug_print_tokens(token_list);
			cmd_list = parse_pipeline(token_list);
			// if (!cmd_list)
			// 	printf("❌ parse_pipeline returned NULL!\n");
			// else
			// 	print_cmd_list(cmd_list);
            execute_pipeline(cmd_list, &shell);
			free_cmd_list(cmd_list);         // free everything safely
			free_tokens(token_list);      // free token list
		}
        add_history(line);
        free(line);
    }
    return (0);
}
