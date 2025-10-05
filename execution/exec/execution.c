/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 13:51:20 by maabdulr          #+#    #+#             */
/*   Updated: 2025/10/05 17:34:42 by ashaheen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	exec_builtin_in_parent(t_cmd *cmd, t_shell *shell)
{
	int	interactive;
	int	status;

	interactive = isatty(0);
	status = 0;
	if (ft_strncmp(cmd->argv[0], "cd", 3) == 0)
		return (exec_cd(cmd->argv, shell));
	if (ft_strncmp(cmd->argv[0], "export", 7) == 0)
		return (exec_export(cmd->argv, shell));
	if (!ft_strncmp(cmd->argv[0], "exit", 5))
		return (exec_exit(cmd, shell, interactive));
	if (ft_strncmp(cmd->argv[0], "unset", 6) == 0)
		return (exec_unset(cmd->argv, shell));
	return (status);
}

int	exec_builtin_in_child(t_cmd *cmd, t_shell *shell)
{
	if (ft_strncmp(cmd->argv[0], "echo", 5) == 0)
		return (exec_echo(cmd->argv));
	if (ft_strncmp(cmd->argv[0], "pwd", 4) == 0)
		return (exec_pwd(cmd->argv));
	if (ft_strncmp(cmd->argv[0], "env", 4) == 0)
		return (exec_env(cmd->argv, shell));
	if (ft_strncmp(cmd->argv[0], "export", 7) == 0)
		return (exec_export(cmd->argv, shell));
	if (ft_strncmp(cmd->argv[0], "cd", 3) == 0)
		return (exec_cd(cmd->argv, shell));
	if (ft_strncmp(cmd->argv[0], "unset", 6) == 0)
		return (exec_unset(cmd->argv, shell));
	if (ft_strncmp(cmd->argv[0], "exit", 5) == 0)
		return (exec_exit(cmd, shell, 0));
	return (0);
}

void	wait_all_children(t_exec *exec, t_shell *shell)
{
	int	i;
	int	st;

	i = 0;
	while (i < exec->cmd_count)
	{
        if (waitpid(exec->pids[i], &st, 0) > 0)
        {
            if (i == exec->cmd_count - 1)
            {
                if (WIFEXITED(st))
                    shell->exit_code = WEXITSTATUS(st);
                else if (WIFSIGNALED(st))
                {
                	if (WTERMSIG(st) == SIGQUIT)
                    ft_putstr_fd("Quit: 3\n", STDERR_FILENO);
                    shell->exit_code = 128 + WTERMSIG(st);
                }
            }
        }
		i++;
	}
}

int	handle_single_cmd(t_cmd *cmd, t_shell *shell)
{
	if (!cmd || cmd->next || !cmd->argv || !cmd->argv[0])
		return (0);
	if (is_variable_assignment(cmd->argv[0]))
	{
		if (handle_variable_assignment(cmd->argv[0], shell))
			shell->exit_code = 0;
		else
			shell->exit_code = 1;
		return (1);
	}
	if (is_parent_builtin(cmd->argv[0]))
	{
		shell->exit_code = exec_builtin_in_parent(cmd, shell);
		return (1);
	}
	return (0);
}

void	execute_pipeline(t_cmd *cmd_list, t_shell *shell)
{
	t_exec	*exec;

	if (!cmd_list)
		return ;
	if (handle_all_heredocs(cmd_list, shell))
		return ;
	if (handle_single_cmd(cmd_list, shell))
		return ;
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	exec = init_exec_struct(cmd_list, shell);
	fork_and_execute_all(cmd_list, exec, shell);
	wait_all_children(exec, shell);
	free_exec_data(exec);
	setup_signals();
}
