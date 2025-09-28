/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fork.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maram <maram@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 15:30:31 by ashaheen          #+#    #+#             */
/*   Updated: 2025/09/24 17:09:44 by maram            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	setup_io(t_cmd *cmd, t_exec *exec, int i)
{
	if (cmd->infile != -1)
		xdup2(cmd->infile, STDIN_FILENO, exec);
	else if (i > 0)
		xdup2(exec->pipes[i - 1][0], STDIN_FILENO, exec);
	if (cmd->outfile != -1)
		xdup2(cmd->outfile, STDOUT_FILENO, exec);
	else if (i < exec->cmd_count - 1)
		xdup2(exec->pipes[i][1], STDOUT_FILENO, exec);
}

void	run_child(t_cmd *cmd, t_exec *exec, t_shell *shell, int i)
{
	char	*path;

	if (cmd->redir_error)
		exit_child(exec, exec->cmd_head, 1);
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	setup_io(cmd, exec, i);
	close_pipe_files_child(exec);
	handle_empty_or_blank(cmd, exec);
	handle_var_cmd(cmd, exec, shell);
	if (is_child_builtin(cmd->argv[0]))
		exit_child(exec, exec->cmd_head, exec_builtin_in_child(cmd, shell));
	path = get_cmd_path(cmd->argv[0], shell, exec, exec->cmd_head);
	exec_or_error(path, cmd, exec, shell);
}

void	fork_and_execute_all(t_cmd *cmd_list, t_exec *exec, t_shell *shell)
{
	int		i;
	t_cmd	*cmd;

	i = 0;
	cmd = cmd_list;
	while (cmd)
	{
		exec->pids[i] = fork();
		if (exec->pids[i] < 0)
			error_exit("fork", exec, exec->cmd_head, 1);
		if (exec->pids[i] == 0)
			run_child(cmd, exec, shell, i);
		close_if_open(&cmd->infile);
		close_if_open(&cmd->outfile);
		cmd = cmd->next;
		i++;
	}
	close_pipe_parent(exec);
}
