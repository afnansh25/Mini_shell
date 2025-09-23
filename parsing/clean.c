
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
