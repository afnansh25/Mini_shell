/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 14:29:10 by ashaheen          #+#    #+#             */
/*   Updated: 2025/08/11 18:03:43 by ashaheen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	**dup_envp(char **src)
{
	int		n;
	int		i;
	char	**dst;

	n = 0;
	while (src[n])
		n++;
	dst = (char **)malloc(sizeof(char *) * (n + 1));
	if (!dst)
		return (NULL);
	i = 0;
	while (i < n)
	{
		dst[i] = ft_strdup(src[i]);
		if (!dst[i])
			return (NULL); // simple version; we can add cleanup later
		i++;
	}
	dst[n] = NULL;
	return (dst);
}

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
	shell.envp = dup_envp(envp);
	if (!shell.envp)
		return (1);
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
