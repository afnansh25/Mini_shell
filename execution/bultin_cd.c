/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bultin_cd.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maabdulr <maabdulr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 13:54:17 by maabdulr          #+#    #+#             */
/*   Updated: 2025/08/23 13:48:09 by maabdulr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

 int  env_count(char **env)
{
    int i;

    i = 0;
    while (env && env[i])
        i++;
    return (i);
}

char    **dup_envp(char **src)
{
    char    **dst;
    int     n;
    int     i;

    n = env_count(src);
    dst = (char **)malloc(sizeof(char *) * (n + 1));
    if (!dst)
        return (NULL);
    i = 0;
    while (i < n)
    {
        dst[i] = ft_strdup(src[i]);
        if (!dst[i])
        {
            // roll back on failure
            while (i > 0)
            {
                i--;
                free(dst[i]);
            }
            free(dst);
            return (NULL);
        }
        i++;
    }
    dst[n] = NULL;
    return (dst);
}

void    free_envp(char **env)
{
    int i;

    if (!env)
        return;
    i = 0;
    while (env[i])
    {
        free(env[i]);
        i++;
    }
    free(env);
}

/*  it search in shell->envp to get the name than '=' than 
returns pointer into envp entry (after '=') or NULL */

char *get_env_value(char *name, t_shell *shell)   
{
    size_t  name_len;
    int     i;

    name_len = ft_strlen(name);
    i = 0;
    while (shell->envp && shell->envp[i])
    {
        if (ft_strncmp(shell->envp[i], name, name_len) == 0
            && shell->envp[i][name_len] == '=')
            return (shell->envp[i] + name_len + 1);
        i++;
    }
    return (NULL);
}
// example
// we have "HOME=/Users/you" and name="HOME", it returns pointer to "/Users/you". Don’t free it


// it build "NAME=value" 
// If key exists, replace that entry; otherwise, allocate a bigger pointer array and append the new entry.

void    update_env_var(char *name, char *value, t_shell *shell)
{
    int     n;        /* reused: name length, then copy index */
    int     i;        /* search index / size */
    char    *entry;   /* "NAME=value" */
    char    *joined;  /* temp for building entry */
    char    **new_env;

    if (!name || !shell)
        return;

    /* if no env yet, create it with a single entry */
    if (!shell->envp)
    {
        entry = ft_strjoin(name, "=");
        if (!entry)
            return;
        if (value)
            joined = ft_strjoin(entry, value);
        else
            joined = ft_strjoin(entry, "");
        free(entry);
        if (!joined)
            return;
        new_env = (char **)malloc(sizeof(char *) * 2);
        if (!new_env)
        {
            free(joined);
            return;
        }
        new_env[0] = joined;
        new_env[1] = NULL;
        shell->envp = new_env;
        return;
    }

    n = (int)ft_strlen(name);
    i = 0;
    while (shell->envp[i]
        && !(ft_strncmp(shell->envp[i], name, n) == 0
        && shell->envp[i][n] == '='))
        i++;

    entry = ft_strjoin(name, "=");
    if (!entry)
        return;
    if (value)
        joined = ft_strjoin(entry, value);
    else
        joined = ft_strjoin(entry, "");
    free(entry);
    if (!joined)
        return;
    entry = joined;

    if (shell->envp[i])
    {
        free(shell->envp[i]);
        shell->envp[i] = entry;
    }
    else
    {
        /* append at end (i already points to NULL) */
        new_env = (char **)malloc(sizeof(char *) * (i + 2));
        if (!new_env)
        {
            free(entry);
            return;
        }
        n = 0;
        while (n < i)
        {
            new_env[n] = shell->envp[n];
            n++;
        }
        new_env[i] = entry;
        new_env[i + 1] = NULL;
        free(shell->envp);
        shell->envp = new_env;
    }
}




// Implement cd
// no args / -- / "" → go to HOME
// - → go to OLDPWD and print the new dir
// expand ~ and ~/path
// chdir(), then update PWD/OLDPWD.
int	exec_cd(char **av, t_shell *shell)
{
	char	*target_dir;
	char	*oldpwd;
	char	*newpwd;
	char	*alloc;
	int		print_newpwd;

	target_dir = NULL;
	alloc = NULL;
	print_newpwd = 0;

	oldpwd = getcwd(NULL, 0);
	if (!oldpwd)
	{
		perror("minishell: cd: getcwd");
		return (1);
	}

	/* ---- choose target ---- */
	if (!av[1] || (av[1][0] == '-' && av[1][1] == '-' && av[1][2] == '\0')
		|| av[1][0] == '\0')                    /* cd, cd --, or cd ""  -> HOME */
	{
		target_dir = get_env_value("HOME", shell);
		if (!target_dir || target_dir[0] == '\0')
		{
			ft_putstr_fd("minishell: cd: HOME not set\n", 2);
			free(oldpwd);
			return (1);
		}
	}
	else if (av[1][0] == '-' && av[1][1] == '\0') /* cd - -> OLDPWD */
	{
		target_dir = get_env_value("OLDPWD", shell);
		if (!target_dir)
		{
			ft_putstr_fd("minishell: cd: OLDPWD not set\n", 2);
			free(oldpwd);
			return (1);
		}
		print_newpwd = 1;
	}
	else
	{
		if (av[1][0] == '~')                      /* ~ expansion */
		{
			char	*home = get_env_value("HOME", shell);

			if (!home || home[0] == '\0')
			{
				ft_putstr_fd("minishell: cd: HOME not set\n", 2);
				free(oldpwd);
				return (1);
			}
			if (av[1][1] == '\0')
				target_dir = home;                 /* "~" */
			else if (av[1][1] == '/')
			{
				alloc = ft_strjoin(home, av[1] + 1); /* "~/x" -> HOME + "/x" */
				if (!alloc)
				{
					free(oldpwd);
					return (1);
				}
				target_dir = alloc;
			}
			else
				target_dir = av[1];               /* "~user" not handled */
		}
		else
			target_dir = av[1];
	}

	/* ---- chdir ---- */
	if (chdir(target_dir) != 0)
	{
		char	*tmp = ft_strjoin("minishell: cd: ", target_dir);

		if (tmp)
		{
			perror(tmp);
			free(tmp);
		}
		else
			perror("minishell: cd");
		free(oldpwd);
		if (alloc)
			free(alloc);
		return (1);
	}

	/* ---- update env (preserve /tmp, not /private/tmp) ---- */
	newpwd = getcwd(NULL, 0);
	if (!newpwd)
	{
		update_env_var("OLDPWD", oldpwd, shell);
		perror("minishell: cd: getcwd");
		free(oldpwd);
		if (alloc)
			free(alloc);
		return (1);
	}

	/* set OLDPWD first */
	update_env_var("OLDPWD", oldpwd, shell);

	/* choose logical PWD string without adding new locals:
	   - no-arg / "--" / ""         -> use target_dir (HOME)
	   - "-"                        -> use target_dir (OLDPWD string)
	   - absolute path ("/...")     -> use av[1] (exact user input)
	   - "~" or "~/" expanded       -> use target_dir (HOME or alloc)
	   - relative path              -> use newpwd (real absolute path) */
	if (!av[1] || (av[1][0] == '-' && av[1][1] == '-' && av[1][2] == '\0')
		|| (av[1] && av[1][0] == '\0'))
		update_env_var("PWD", target_dir, shell);
	else if (av[1][0] == '-' && av[1][1] == '\0')
		update_env_var("PWD", target_dir, shell);
	else if (av[1][0] == '/')
		update_env_var("PWD", av[1], shell);
	else if (av[1][0] == '~')
		update_env_var("PWD", target_dir, shell);
	else
		update_env_var("PWD", newpwd, shell);

	if (print_newpwd)
		printf("%s\n", newpwd);

	free(oldpwd);
	free(newpwd);
	if (alloc)
		free(alloc);
	return (0);
}
