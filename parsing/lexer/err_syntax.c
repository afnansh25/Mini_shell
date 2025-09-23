
#include "minishell.h"

int	err_newline(void)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `newline'\n", 2);
	return (0);
}

int	err_pipe(void)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
	return (0);
}

int	err_tok(const char *s)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
	ft_putstr_fd((char *)s, 2);
	ft_putstr_fd("'\n", 2);
	return (0);
}

int	err_event(void)
{
	ft_putstr_fd("minishell: !: event not found\n", 2);
	return (0);
}

void	semicolon_error(t_token **head, t_shell *shell, char next)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
	if (next == ';')
		ft_putstr_fd(";;", 2);
	else
		ft_putstr_fd(";", 2);
	ft_putstr_fd("'\n", 2);
	shell->exit_code = 2;
	free_tokens(*head);
	*head = NULL;
}