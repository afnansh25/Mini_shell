
#include "minishell.h"

void	add_arg(t_arg **list, char *val)
{
    t_arg *new;
	t_arg *last;

	new = malloc(sizeof(t_arg));
	if (!new)
		return ; // or handle error
	new->value = val;
	new->next = NULL;

	if (!*list)
		*list = new;
	else
	{
		last = *list;
		while (last->next)
			last = last->next;
		last->next = new;
	}
}

char	**arg_list_to_array(t_arg *list)
{
    char **arr;
    t_arg *tmp;
	int count;
    int i;

    tmp = list;
    count = 0;
    while(tmp)
    {
        count++;
        tmp = tmp->next;
    }
	arr = malloc(sizeof(char *) * (count + 1));
	if (!arr)
		return (NULL);
	i = 0;
	while (list && i < count)
	{
		arr[i++] = list->value;  // reuse already malloc'd string
		tmp = list;
		list = list->next;
		free(tmp); // free the node (not limiter string)
	}
    arr[i] = NULL;
	return (arr);
}

void	free_arg_list(t_arg *list)
{
	t_arg *tmp;

	while (list)
	{
		tmp = list->next;
		free(list);
		list = tmp;
	}
}

void	handle_args(t_cmd *cmd, t_token **token_ptr)
{
	t_arg *arg_list;

	arg_list = NULL;
	while (*token_ptr && (*token_ptr)->type == WORD)
	{
		add_arg(&arg_list, (*token_ptr)->value);
		*token_ptr = (*token_ptr)->next;
	}
	cmd->argv = arg_list_to_array(arg_list);
} 