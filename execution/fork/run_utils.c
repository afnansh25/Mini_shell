/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maabdulr <maabdulr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 13:23:30 by codespace         #+#    #+#             */
/*   Updated: 2025/09/25 17:36:52 by maabdulr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_plain_var_reference(const char *s)
{
	int	i;

	if (!s || s[0] != '$')
		return (0);
	if (s[1] == '\0')
		return (0);
	if (s[1] == '?' && s[2] == '\0')
		return (1);
	i = 1;
	while (s[i])
	{
		if (!ft_isalnum((unsigned char)s[i]) && s[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

int	is_blank(const char *s)
{
	int	i;

	if (!s)
		return (1);
	i = 0;
	while (s[i])
	{
		if (!ft_isspace((unsigned char)s[i]))
			return (0);
		i++;
	}
	return (1);
}

void	exec_or_error(char *path, t_cmd *cmd, t_exec *exec, t_shell *shell)
{
	int	exit_code;

	if (execve(path, cmd->argv, shell->envp) == -1)
	{
		if (errno == ENOENT)
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(cmd->argv[0], 2);
			ft_putstr_fd(": command not found\n", 2);
			exit_code = 127;
		}
		else
		{
			perror(cmd->argv[0]);
			exit_code = 126;
		}
		free(path);
		error_exit(cmd->argv[0], exec, exec->cmd_head, exit_code);
	}
}

void	handle_empty_or_blank(t_cmd *cmd, t_exec *exec)
{
	if (!cmd->argv || !cmd->argv[0])
		exit(0);
	if (is_blank(cmd->argv[0]))
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd->argv[0], 2);
		ft_putstr_fd(": command not found\n", 2);
		exit_child(exec, exec->cmd_head, 127);
	}
}

void	handle_var_cmd(t_cmd *cmd, t_exec *exec, t_shell *shell)
{
	char	*val;

	if (!is_plain_var_reference(cmd->argv[0]))
		return ;
	val = get_var_value(cmd->argv[0] + 1, shell);
	ft_putstr_fd("minishell: ", 2);
	if (val)
	{
		ft_putstr_fd(val, 2);
		ft_putstr_fd(": command not found\n", 2);
		free(val);
	}
	else
	{
		ft_putstr_fd(cmd->argv[0], 2);
		ft_putstr_fd(": unbound variable\n", 2);
	}
	exit_child(exec, exec->cmd_head, 127);
}
