/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 14:29:10 by ashaheen          #+#    #+#             */
/*   Updated: 2025/07/25 23:04:08 by maram            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_cmd_list(t_cmd *cmd)
{
	int i;
	int j;

	i = 1;
	while (cmd)
	{
		printf("🟢 Command #%d:\n", i);
		j = 0;
		if (cmd->argv)
		{
			printf("  argv: ");
			while (cmd->argv[j])
				printf("[%s] ", cmd->argv[j++]);
			printf("\n");
		}
		if (!cmd->argv)
			printf("⚠️ argv is NULL for command #%d\n", i);
		if (cmd->infile != -1)
			printf("  infile: %d\n", cmd->infile);
		if (cmd->outfile != -1)
			printf("  outfile: %d (%s)\n", cmd->outfile, cmd->append ? "append" : "truncate");
		for (j = 0; j < cmd->n_heredocs; j++)
			printf("  heredoc[%d]: \"%s\" (quoted=%d)\n", j, cmd->heredocs[j].limiter, cmd->heredocs[j].quoted);
		cmd = cmd->next;
		i++;
	}
}

int main()
{
    char 	*line;
    t_token	*token_list;
	t_cmd	*cmd_list;

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
			cmd_list = parse_pipeline(token_list);
			if (!cmd_list)
				printf("❌ parse_pipeline returned NULL!\n");
			else
				print_cmd_list(cmd_list);
			//execution
			free_cmd_list(cmd_list);         // free everything safely
			free_tokens(token_list);         // free token list
		}
        add_history(line);
        free(line);
    }
    return (0);
}
