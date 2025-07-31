
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

void	free_cmd_list(t_cmd *cmd_list)
{
    t_cmd   *tmp;
    int i;

    while (cmd_list)
    {
        if(cmd_list->argv)
            free_arr(cmd_list->argv);
        if (cmd_list->heredocs)
		{
			i = 0;
			while (i < cmd_list->n_heredocs)
				free(cmd_list->heredocs[i++].limiter);
			free(cmd_list->heredocs);
		}
        if(cmd_list->infile != -1)
            close(cmd_list->infile);
        if(cmd_list->outfile != -1)
            close(cmd_list->outfile);
		tmp = cmd_list->next;
		free(cmd_list);
		cmd_list = tmp;
    }
}
