/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc_ex.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 14:27:52 by ashaheen          #+#    #+#             */
/*   Updated: 2025/08/31 11:32:47 by ashaheen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_var_char(char c)
{
	if (ft_isalnum((unsigned char)c) || c == '_')
		return (1);
	return (0);
}

char	*expand_line_heredoc(char *line, t_shell *shell)
{
	char	*res;
	char	*tmp;
	int		i;

	i = 0;
	res = ft_strdup("");
	while (line[i])
	{
		if (line[i] == '$' && (line[i + 1] == '?' || is_var_char(line[i + 1])))
		{
			tmp = handle_dollar(line, &i, shell);
			res = append_str(res, tmp);
			free(tmp);
		}
		else
		{
			res = append_char(res, line[i]);
			i++;
		}
	}
	return (res);
}

void	read_heredoc_input(int write_fd, char *limiter, int quoted, t_shell *shell)
{
	char	*line;
	char	*expand;
	size_t	len;
	
	signal(SIGINT, sigint_heredoc_handler);
	len = ft_strlen(limiter);
	while (1)
	{
		line = readline("> ");
		if (!line)
			break ;
		if (ft_strncmp(line, limiter, len) == 0)
		{
			free(line);
			break ;
		}
		if (quoted == 0)
		{
			expand = expand_line_heredoc(line, shell);
			write(write_fd, expand, ft_strlen(expand));
        	write(write_fd, "\n", 1);
			free(expand);
			free(line);
		}
		else
		{
			write(write_fd, line, ft_strlen(line));
			write(write_fd, "\n", 1);
			free(line);
		}
	}
	close(write_fd);
	exit(0);
}

int	handle_here_doc(t_heredoc *hdoc, t_shell *shell, t_cmd *cmd_list)
{
	int	pipe_fd[2];
	int	pid;
	int status;

	if (pipe(pipe_fd) == -1)
		error_exit("pipe", NULL, cmd_list, 1);
	pid = fork();
	if (pid < 0)
		error_exit("fork", NULL, cmd_list, 1);
	if (pid == 0)
	{
		close(pipe_fd[0]);
		read_heredoc_input(pipe_fd[1], hdoc->limiter, hdoc->quoted, shell);
	}
	else
	{
		close(pipe_fd[1]);
		waitpid(pid, &status, 0);
		if ((WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
				|| (WIFEXITED(status) && WEXITSTATUS(status) == 130))
		{
				close(pipe_fd[0]);
				return (-1); // signal parent to cancel pipeline
		}
		return (pipe_fd[0]); // return read end
}
return (-1); // fallback
}

int	process_all_heredocs(t_cmd *cmd, t_shell *shell)
{
int		i;
int		fd;

i = 0;
while (i < cmd->n_heredocs)
{
fd = handle_here_doc(&cmd->heredocs[i], shell, cmd);
if (fd == -1)
{
	shell->exit_code = 130;
	return (1);
}
if (cmd->infile != -1)
	close(cmd->infile);
cmd->infile = fd;
i++;
}
return (0);
}

int handle_all_heredocs(t_cmd *cmd_list, t_shell *shell)
{
t_cmd	*cmd;

cmd = cmd_list;
while (cmd)
{
if (cmd->n_heredocs > 0)
{
	if (process_all_heredocs(cmd, shell))
		return (1);
}
cmd = cmd->next;
}
return (0);
}