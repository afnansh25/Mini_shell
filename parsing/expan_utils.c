//header
#include "minishell.h"

char *append_str(char *str, char *suffix)
{
	char *new;

	new = ft_strjoin(str, suffix);
	free(str);
	return (new);
}

char *append_char(char *str, char c)
{
	int		len;
	char	*new;

	len = ft_strlen(str);
	new = malloc(len + 2);
	if (!new)
		return (NULL);
	ft_memcpy(new, str, len);
	new[len] = c;
	new[len + 1] = '\0';
	free(str);
	return (new);
}