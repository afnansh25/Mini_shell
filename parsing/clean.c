
#include "minishell.h"

//debug
void	print_tokens(t_token *tok)
{
	while (tok)
	{
		printf("[%s] '%s'\n", tok->type == CMD ? "CMD" :
			tok->type == ARG ? "ARG" :
			tok->type == PIPE ? "PIPE" :
			tok->type == WORD ? "WORD" : "OTHER", tok->value);
		tok = tok->next;
	}
}
//debug
void	print_cmd_list(t_cmd *cmd)
{
	int	i;
	int	cmd_idx;

	cmd_idx = 0;
	while (cmd)
	{
		printf("🔧 CMD #%d:\n", cmd_idx++);
		if (!cmd->argv)
			printf("  argv: NULL\n");
		else
		{
			i = 0;
			while (cmd->argv[i])
			{	
				printf("  argv[%d] = %s\n", i, cmd->argv[i]);
				i++;
			}
		}
		printf("  infile: %d\n", cmd->infile);
		printf("  outfile: %d\n", cmd->outfile);
		printf("  append: %d\n", cmd->append);
		printf("  heredoc count: %d\n", cmd->n_heredocs);
		cmd = cmd->next;
	}
}

void debug_print_tokens(t_token *tok)
{
	while (tok)
	{
		if (tok->type == CMD)
			printf("[CMD] ");
		else if (tok->type == ARG)
			printf("[ARG] ");
		else if (tok->type == PIPE)
			printf("[PIPE] ");
		else if (tok->type == WORD)
			printf("[WORD] ");
		else
			printf("[OTHER] ");
		printf("'%s'\n", tok->value);
		tok = tok->next;
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
void	*free_arr(char **arr)
{
	size_t	i;

	if (!arr)
		return (NULL);
	i = 0;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
	return (NULL);
}

// void	free_cmd_list(t_cmd *cmd_list)
// {
//     t_cmd   *tmp;
//     int i;

//     while (cmd_list)
//     {
//         if(cmd_list->argv)
//             free_arr(cmd_list->argv);
//         if (cmd_list->heredocs)
// 		{
// 			i = 0;
// 			while (i < cmd_list->n_heredocs)
// 				free(cmd_list->heredocs[i++].limiter);
// 			free(cmd_list->heredocs);
// 		}
//         if(cmd_list->infile != -1)
//             close(cmd_list->infile);
//         if(cmd_list->outfile != -1)
//             close(cmd_list->outfile);
// 		tmp = cmd_list->next;
// 		free(cmd_list);
// 		cmd_list = tmp;
//     }
// }

void	cleanup_on_exit(t_shell *sh, t_cmd *cmd, t_exec *ex)
{
	if (ex)
		free_exec_data(ex);
	if (cmd)
		free_cmd_list(cmd);
	if (sh && sh->envp)
	{
		free_envp(sh->envp);
		sh->envp = NULL;
	}
	rl_clear_history();
}
