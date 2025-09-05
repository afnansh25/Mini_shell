/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 15:39:55 by ashaheen          #+#    #+#             */
/*   Updated: 2025/09/05 15:50:54 by ashaheen         ###   ########.fr       */
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

int	hd_should_stop(char *line, char *lim)
{
	size_t	len;

	if (!line)
		return (1);
	len = ft_strlen(lim);
	if (ft_strncmp(line, lim, len) == 0 && line[len] == '\0')
	{
		free(line);
		return (1);
	}
	return (0);
}

int	hd_wait_and_check(pid_t pid, int rfd, t_shell *shell)
{
	int	status;

	while (waitpid(pid, &status, 0) == -1)
	{
		if (errno != EINTR)
		{
			close(rfd);
			return (-1);
		}
	}
	if ((WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
		|| (WIFEXITED(status) && WEXITSTATUS(status) == 130))
	{
		close(rfd);
		shell->exit_code = 130;
		return (-1);
	}
	return (rfd);
}