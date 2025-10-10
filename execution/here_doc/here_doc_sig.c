/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc_sig.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 15:39:08 by ashaheen          #+#    #+#             */
/*   Updated: 2025/10/10 19:17:18 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	*heredoc_fd_ptr(void)
{
	static int	fd = -1;

	return (&fd);
}

void	hd_set_write_fd(int fd)
{
	int	*p;

	p = heredoc_fd_ptr();
	*p = fd;
}

void	sigint_heredoc_handler(int sig)
{
	int	*p;

	(void)sig;
	g_signo = SIGINT;
	p = heredoc_fd_ptr();
	if (*p != -1)
		close(*p);
	close(STDIN_FILENO);
}

void	hd_install_sig(void)
{
	struct sigaction	sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = sigint_heredoc_handler;
	sigaction(SIGINT, &sa, NULL);
}
