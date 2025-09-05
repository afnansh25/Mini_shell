/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc_ex.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 14:27:52 by ashaheen          #+#    #+#             */
/*   Updated: 2025/09/05 15:19:08 by ashaheen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int  *heredoc_fd_ptr(void)
{
    static int      fd = -1;

    return (&fd);
}


void    sigint_heredoc_handler(int sig)
{
    int *p;
    (void)sig;
	g_signo = SIGINT;
    p = heredoc_fd_ptr();
    if (*p != -1)
        {close(*p);}          // close write end so parent sees EOF
	close(STDIN_FILENO); 
    write(1, "\n", 1);
    // exit(130);             // child exits with 130 (SIGINT)
}

// void    sigint_heredoc_handler(int sig)
// {
//         int     fd;

//         (void)sig;
//         fd = *heredoc_fd_ptr();
//         if (fd != -1)
//             close(fd);
//         write(1, "\n", 1);
//         exit(1);
// }

void	cleanup_session(t_shell *sh, t_cmd **head, int final)
{
	if (head && *head)
	{
		free_cmd_list(*head);
		*head = NULL;
	}
	if (final && sh)
	{
		if (sh->envp) 
		{ 
			free_arr(sh->envp); 
			sh->envp = NULL; 
		}
		if (sh->exp)  
		{ 
			free_arr(sh->exp);  
			sh->exp  = NULL; 
		}
		rl_clear_history();
	}
}

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

void	read_heredoc_input(int write_fd, t_heredoc *hdoc, t_shell *shell, t_cmd *cmd_list)
{
	char	*line;
	char	*expand;
	int 	code; 
	size_t	len;
	
	*heredoc_fd_ptr() = write_fd;
	signal(SIGINT, sigint_heredoc_handler); // Use default SIGINT behavior
	len = ft_strlen(hdoc->limiter);
	while (1)
	{
		line = readline("> ");
		if (!line) // EOF (Ctrl+D)
		{
			break;
		}
		if (ft_strncmp(line, hdoc->limiter, len) == 0 && line[len] == '\0')
		{
			free(line);
			break ;
		}
		if (hdoc->quoted == 0)
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
	*heredoc_fd_ptr() = -1;
	{
		code = 0;            /* initialization */
	
		if (g_signo == SIGINT)
			code = 130;
	
		/* free the CHILD copies here */
		free_cmd_list(cmd_list);
		free_envp(shell->envp);
	
		g_signo = 0;         /* optional: reset for safety */
		exit(code);
	}
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
		read_heredoc_input(pipe_fd[1], hdoc, shell, cmd_list);
		free_envp(shell->envp);
		free_cmd_list(cmd_list);
		exit(0);
	}
	else
	{
		close(pipe_fd[1]);
		while (waitpid(pid, &status, 0) == -1)
		{
			if (errno != EINTR)
			{
				close(pipe_fd[0]);
				return (-1);
			}
		}
		if ((WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
			|| (WIFEXITED(status) && WEXITSTATUS(status) == 130))
		{
			close(pipe_fd[0]);
			shell->exit_code = 130;
			return (-1); // signal parent to cancel pipeline
		}
		return (pipe_fd[0]); // return read end
	}
	return (-1); // fallback
}


int	process_all_heredocs(t_cmd *cmd, t_cmd *cmd_list_head, t_shell *shell)
{
	int		i;
	int		fd;

	i = 0;
	while (i < cmd->n_heredocs)
	{
		fd = handle_here_doc(&cmd->heredocs[i], shell, cmd_list_head);
		if (fd == -1)
		{
			shell->exit_code = 130;
			if (cmd->infile != -1)
			{
				close(cmd->infile);
				cmd->infile = -1;
			}
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
			if (process_all_heredocs(cmd, cmd_list, shell))
				return (1);
		}
		cmd = cmd->next;
	}
	return (0);
}