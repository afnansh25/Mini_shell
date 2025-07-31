/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 19:14:11 by ashaheen          #+#    #+#             */
/*   Updated: 2025/01/10 19:14:12 by ashaheen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strchr(const char *s, int c)
{
	size_t				i;
	unsigned const char	*str;
	unsigned char		letter;

	i = 0;
	str = (unsigned const char *)s;
	letter = (unsigned char)c;
	while (str[i])
	{
		if (str[i] == letter)
			return ((char *)(s + i));
		i++;
	}
	if (letter == '\0')
		return ((char *)(s + i));
	return (NULL);
}
