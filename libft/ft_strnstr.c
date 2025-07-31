/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 19:15:48 by ashaheen          #+#    #+#             */
/*   Updated: 2025/01/11 13:54:11 by ashaheen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *haystack, const char *needle, size_t len)
{
	unsigned const char	*hay;
	unsigned const char	*need;
	size_t				i;
	size_t				j;

	i = 0;
	hay = (unsigned const char *)haystack;
	need = (unsigned const char *)needle;
	if (need[i] == '\0')
		return ((char *)haystack);
	while (i < len && hay[i])
	{
		j = 0;
		while (need[j] && ((i + j) < len))
		{
			if (hay[i + j] != need[j])
				break ;
			j++;
		}
		if (need[j] == '\0')
			return ((char *)(haystack + i));
		i++;
	}
	return (NULL);
}
