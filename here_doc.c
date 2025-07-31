
#include "minishell.h"

void	add_heredoc(t_heredoc_node **list, char *limiter, int quoted)
{
	t_heredoc_node *new;
	t_heredoc_node *last;

	new = malloc(sizeof(t_heredoc_node));
	if (!new)
		return ; // or handle error
	new->limiter = rmv_quotes(limiter);
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

void	handle_heredoc(t_cmd *cmd, t_token **token_ptr)
{
    t_heredoc_node *temp_heredoc_list;
    int heredoc_count;
    int quoted;

    temp_heredoc_list = NULL;
    heredoc_count = 0;
    while(*token_ptr && (*token_ptr)->type == HEREDOC)
    {
        *token_ptr = (*token_ptr)->next;
        if (!*token_ptr)
        {
			free_heredoc_list(temp_heredoc_list);
			return ;
		}
        if ((*token_ptr)->value[0] == '\'' || (*token_ptr)->value[0] == '"')
            quoted = 1;
        else
            quoted = 0;
        add_heredoc(&temp_heredoc_list, (*token_ptr)->value, quoted);
        heredoc_count++;
        *token_ptr = (*token_ptr)->next;
    }
    cmd->heredocs = heredoc_list_to_array(temp_heredoc_list, heredoc_count);
    cmd->n_heredocs = heredoc_count;
}
