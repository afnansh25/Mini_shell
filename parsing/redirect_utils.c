#include "minishell.h"

void	errno_msg(const char *s)
{
	ft_putstr_fd("minishell: ", 2);
	perror(s);
}

int	open_read_fd(t_cmd *cmd, const char *filename)
{
	if (cmd->infile != -1)
		close(cmd->infile);
	cmd->infile = open(filename, O_RDONLY);
	if (cmd->infile == -1)
	{
		errno_msg(filename);
		cmd->redir_error = 1;
		return (0);
	}
	if (fcntl(cmd->infile, F_SETFD, FD_CLOEXEC) == -1)
	{
		errno_msg("fcntl");
		close(cmd->infile);
		cmd->infile = -1;
		cmd->redir_error = 1;
		return (0);
	}
	return (1);
}

void	handle_redir_in(t_cmd *cmd, t_token **token_ptr)
{
	char	*filename;

	if (!*token_ptr || !(*token_ptr)->next)
		return ;
	*token_ptr = (*token_ptr)->next;
	filename = ft_strdup((*token_ptr)->value);
	if (!filename)
	{
		cmd->redir_error = 1;
		return ;
	}
	open_read_fd(cmd, filename);
	free(filename);
	*token_ptr = (*token_ptr)->next;
}


void    handle_redir_out(t_cmd *cmd, t_token **token_ptr)
{
    char    *filename;

    if (!(*token_ptr) || !(*token_ptr)->next)
        return ;
    *token_ptr = (*token_ptr)->next;
    filename = ft_strdup((*token_ptr)->value);
    if (!filename)
        return ;
    if (cmd->outfile != -1)
        close(cmd->outfile);
    cmd->outfile = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (cmd->outfile == -1)
    {
		ft_putstr_fd("minishell: ", 2);
        perror(filename);
        cmd->redir_error = 1;
    }
    free(filename);
    *token_ptr = (*token_ptr)->next;
}


int	open_append_fd(t_cmd *cmd, const char *filename)
{
	if (cmd->outfile != -1)
		close(cmd->outfile);
	cmd->append = 1;
	cmd->outfile = open(filename, O_CREAT | O_WRONLY | O_APPEND, 0644);
	if (cmd->outfile == -1)
	{
		errno_msg(filename);
		cmd->redir_error = 1;
		return (0);
	}
	if (fcntl(cmd->outfile, F_SETFD, FD_CLOEXEC) == -1)
	{
		errno_msg("fcntl");
		close(cmd->outfile);
		cmd->outfile = -1;
		cmd->redir_error = 1;
		return (0);
	}
	return (1);
}
