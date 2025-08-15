
#include "minishell.h"

int	add_heredoc(t_heredoc_node **list, char *limiter, int quoted)
{
	t_heredoc_node *new;
	t_heredoc_node *last;

	new = malloc(sizeof(t_heredoc_node));
	if (!new)
		return(0) ; // or handle error
	new->limiter = rmv_quotes(limiter);
	if (!new->limiter)
	{
		free(new);
		return(0) ;
	}
	new->quoted = quoted;
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
	return (1);
}

t_heredoc *heredoc_list_to_array(t_heredoc_node *list, int count)
{
	t_heredoc *arr;
    t_heredoc_node *tmp;
	int i;

	arr = malloc(sizeof(t_heredoc) * count);
	if (!arr)
		return (NULL);
	i = 0;
	while (list && i < count)
	{
		arr[i].limiter = list->limiter;  // reuse already malloc'd string
		arr[i].quoted = list->quoted;
		tmp = list;
		list = list->next;
		free(tmp); // free the node (not limiter string)
		i++;
	}
	return (arr);
}

void	free_heredoc_list(t_heredoc_node *list)
{
	t_heredoc_node *tmp;

	while (list)
	{
		tmp = list->next;
		free(list->limiter);
		free(list);
		list = tmp;
	}
}

t_heredoc_node *collect_heredocs(t_token **token_ptr, int *count)
{
	t_heredoc_node *list;
	 int quoted;
 
	list = NULL;
	*count = 0;
	while (*token_ptr && (*token_ptr)->type == HEREDOC)
	{
		*token_ptr = (*token_ptr)->next;
		if (!*token_ptr)
			break;
        if ((*token_ptr)->quote == SINGLE_QUOTE || (*token_ptr)->quote == DOUBLE_QUOTE)
            quoted = 1;
        else
            quoted = 0;
		if (add_heredoc(&list, (*token_ptr)->value, quoted))
			(*count)++;
		 *token_ptr = (*token_ptr)->next;
	}
	return list;
}

void handle_heredoc(t_cmd *cmd, t_token **token_ptr)
{
	t_heredoc_node *list;
	int count;
 
	list = collect_heredocs(token_ptr, &count);
	cmd->n_heredocs = 0;
	if (!list)
	{
		cmd->heredocs = NULL;
		return;
	}
	cmd->heredocs = heredoc_list_to_array(list, count);
	if (!cmd->heredocs)
	{
		free_heredoc_list(list);
		return;
	}
	cmd->n_heredocs = count;
}

 