/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 19:12:40 by ashaheen          #+#    #+#             */
/*   Updated: 2025/01/10 19:12:40 by ashaheen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	unsigned const char	*str;
	unsigned char		letter;
	size_t				i;

	i = 0;
	str = (unsigned const char *)s;
	letter = (unsigned char)c;
	if (n == 0)
		return (NULL);
	while (i < n)
	{
		if (str[i] == letter)
			return ((void *)(s + i));
		i++;
	}
	return (NULL);
}
