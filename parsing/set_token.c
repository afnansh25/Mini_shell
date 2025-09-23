#include "minishell.h"

int	is_redir_type(int t)
{
	return (t == REDIR_IN || t == REDIR_OUT || t == REDIR_APPEND || t == HEREDOC);
}

void	handle_pipe(t_token **cur, int *is_cmd)
{
	*is_cmd = 1;
	*cur = (*cur)->next;
}

void	skip_redir_pair(t_token **cur)
{
	*cur = (*cur)->next;
	if (*cur)
		*cur = (*cur)->next;
}

void	handle_word(t_token **cur, int *is_cmd)
{
	if (*is_cmd)
	{
		(*cur)->type = CMD;
		*is_cmd = 0;
	}
	else
		(*cur)->type = ARG;
	*cur = (*cur)->next;
}

void	set_token_types(t_token *tokens)
{
	t_token	*cur;
	int		is_cmd;

	cur = tokens;
	is_cmd = 1;
	while (cur)
	{
		if (cur->type == PIPE)
			handle_pipe(&cur, &is_cmd);
		else if (is_redir_type(cur->type))
			skip_redir_pair(&cur);
		else if (cur->type == WORD)
			handle_word(&cur, &is_cmd);
		else
			cur = cur->next;
	}
}
