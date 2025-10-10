/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc_ex.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 14:27:52 by ashaheen          #+#    #+#             */
/*   Updated: 2025/10/10 19:20:40 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	hd_write_line(int fd, char *line, t_shell *shell, int quoted)
{
	char	*exp;

	if (quoted == 0)
	{
		exp = expand_line_heredoc(line, shell);
		write(fd, exp, ft_strlen(exp));
		write(fd, "\n", 1);
		free(exp);
	}
	else
	{
		write(fd, line, ft_strlen(line));
		write(fd, "\n", 1);
	}
	free(line);
}

void	read_heredoc_input(int write_fd, t_heredoc *hdoc, t_shell *shell,
	t_cmd *cmd_list)
{
	char	*line;
	int		code;

	hd_set_write_fd(write_fd);
	hd_install_sig();
	while (1)
	{
		line = readline("> ");
		if (hd_should_stop(line, hdoc->limiter))
			break ;
		hd_write_line(write_fd, line, shell, hdoc->quoted);
	}
	close_if_open(&write_fd);
	hd_set_write_fd(-1);
	code = 0;
	if (g_signo == SIGINT)
		code = 130;
	free_cmd_list(cmd_list);
	free_envp(shell->envp);
	g_signo = 0;
	exit(code);
}

int	handle_here_doc(t_heredoc *hdoc, t_shell *shell, t_cmd *cmd_list)
{
	int		pipe_fd[2];
	pid_t	pid;

	if (pipe(pipe_fd) == -1)
		error_exit("pipe", NULL, cmd_list, 1);
	pid = fork();
	if (pid < 0)
		error_exit("fork", NULL, cmd_list, 1);
	if (pid == 0)
	{
		close_if_open(&pipe_fd[0]);
		read_heredoc_input(pipe_fd[1], hdoc, shell, cmd_list);
	}
	close_if_open(&pipe_fd[1]);
	return (hd_wait_and_check(pid, pipe_fd[0], shell));
}

int	process_all_heredocs(t_cmd *cmd, t_cmd *head, t_shell *shell)
{
	int	i;
	int	fd;

	i = 0;
	while (i < cmd->n_heredocs)
	{
		fd = handle_here_doc(&cmd->heredocs[i], shell, head);
		if (fd == -1)
		{
			if (cmd->infile != -1)
			{
				close(cmd->infile);
				cmd->infile = -1;
			}
			return (1);
		}
		close_if_open(&cmd->infile);
		cmd->infile = fd;
		i++;
	}
	return (0);
}

int	handle_all_heredocs(t_cmd *cmd_list, t_shell *shell)
{
	t_cmd	*cmd;

	cmd = cmd_list;
	while (cmd)
	{
		if (cmd->n_heredocs > 0)
		{
			if (process_all_heredocs(cmd, cmd_list, shell))
				return (1);
		}
		cmd = cmd->next;
	}
	return (0);
}
