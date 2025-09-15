/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   repl.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 15:02:43 by codespace         #+#    #+#             */
/*   Updated: 2025/09/15 15:02:59 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	shell_shutdown(t_shell *sh)
{
	free_envp(sh->envp);
	free_arr(sh->exp);
}

static int	preprocess_tokens(t_token **ptok, t_shell *sh)
{
	if (!validate_syntax(*ptok))
	{
		sh->exit_code = 2;
		return (0);
	}
	set_token_types(*ptok);
	expand_token_list(*ptok, sh);
	if (remove_empty_tokens(ptok, sh))
		return (0);
	if (!*ptok)
		return (0);
	set_token_types(*ptok);
	return (1);
}

void	process_line(char *line, t_shell *sh)
{
	t_token	*tok;
	t_cmd	*cmd;

	tok = NULL;
	cmd = NULL;
	tokens(line, &tok, sh);
	if (!tok)
		return ;
	if (!preprocess_tokens(&tok, sh))
	{
		if (tok)
			free_tokens(tok);
		return ;
	}
	cmd = parse_pipeline(tok);
	free_tokens(tok);
	execute_pipeline(cmd, sh);
	free_cmd_list(cmd);
}

void	repl_loop(t_shell *sh)
{
	char	*line;

	while (1)
	{
		setup_signals();
		line = readline("pink_bow🎀$ ");
		if (!line)
		{
			ft_putendl_fd("exit", 1);
			break ;
		}
		if (*line)
		{
			add_history(line);
			process_line(line, sh);
		}
		free(line);
	}
}
