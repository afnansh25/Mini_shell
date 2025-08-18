/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bultin_cd.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maabdulr <maabdulr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 13:54:17 by maabdulr          #+#    #+#             */
/*   Updated: 2025/08/13 17:46:37 by maabdulr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <string.h>


char *get_env_value(char *name, t_shell *shell)
{
    int i = 0;
    size_t name_len;
    
    i = 0;
    name_len = ft_strlen(name);
    while (shell->envp[i])
    {
        if (ft_strncmp(shell->envp[i], name, name_len) == 0 &&
            shell->envp[i][name_len] == '=')
        {
            return shell->envp[i] + name_len + 1;
        }
        i++;
    }
    return NULL;
}

// this is a helper function to update environment variables

void update_env_var(char *name, char *value, t_shell *shell)
{
    int i;
    char *new_entry;
    char **new_env;
    size_t name_len;

    i = 0;
    name_len = ft_strlen(name);
    // Find existing variable
    while (shell->envp[i] && 
           !(ft_strncmp(shell->envp[i], name, name_len) == 0 &&
             shell->envp[i][name_len] == '=')) {
        i++;
    }
    // Create new entry: "NAME=value"
    char *temp = ft_strjoin(name, "=");
    if (!temp) return;
    new_entry = ft_strjoin(temp, value);
    free(temp);
    
    if (shell->envp[i]) {
        // Update existing entry
        free(shell->envp[i]);
        shell->envp[i] = new_entry;
    } else {
        // Create new entry at end of envp array
        int count = 0;
        while (shell->envp[count]) count++;
        
        new_env = malloc(sizeof(char *) * (count + 2));
        if (!new_env) {
            free(new_entry);
            return;
        }
        
        int j = 0;
        while (j < count) {
            new_env[j] = shell->envp[j];
            j++;
        }
        new_env[j] = new_entry;
        new_env[j+1] = NULL;
        
        free(shell->envp);
        shell->envp = new_env;
    }
}
// so here i will main cd function with proper return type
int exec_cd(char **av, t_shell *shell)
{
    char    *target_dir;
    char    *oldpwd;
    char    *newpwd;
    int     print_newpwd;
    int     i;
    
    target_dir = NULL;
    oldpwd = NULL;
    newpwd = NULL;
    print_newpwd = 0;
    i = 0;
    
    //1- so the first step -> argument validation 
    while (av[i])
    i++;
    // 2- secand step -> Get current directory before change
    oldpwd = getcwd(NULL, 0);
    if (!oldpwd)
    {
        perror("minishell: cd: getcwd error");
        return (1);
    }
    
     // Step 3: Determine target directory
    //  if (!av[1]) ------
    // {
    //     // Case: cd (no arguments) → go to HOME
    //     target_dir = get_env_value("HOME", shell);
    //     if (!target_dir) {
    //         ft_putstr_fd("minishell: cd: HOME not set\n", 2);
    //         free(oldpwd);
    //         return (1);
    //     }
    // }
    if (!av[1] && target_dir && target_dir[0] == '\0') 
    {
        ft_putstr_fd("minishell: cd: HOME not set\n", 2);
        free(oldpwd);
        return (1);
    }
    else if (strcmp(av[1], "-") == 0) 
    {
        // Case: cd - → go to OLDPWD
        target_dir = get_env_value("OLDPWD", shell);
        if (!target_dir) {
            ft_putstr_fd("minishell: cd: OLDPWD not set\n", 2);
            free(oldpwd);
            return (1);
        }
        print_newpwd = 1; // Flag to print directory after change
    }
    else 
    {
        // Case: cd with path argument
        target_dir = av[1];
    }
    // Step 4: Change directory
    if (chdir(target_dir) != 0)
    {
        perror("minishell: cd");
        free(oldpwd);
        return (1);
    }

    // Step 5: Get new directory after change
    newpwd = getcwd(NULL, 0);
    if (!newpwd) 
    {
        perror("minishell: cd: getcwd error");
        free(oldpwd);
        return (1);
    }

    // Step 6: Update environment variables
    update_env_var("OLDPWD", oldpwd, shell);
    update_env_var("PWD", newpwd, shell);

    // Step 7: Print new directory for cd -
    if (print_newpwd)
    {
        printf("%s\n", newpwd);
    }

    // Step 8: Cleanup
    free(oldpwd);
    free(newpwd);
    return (0);
}
