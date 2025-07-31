/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 19:16:15 by ashaheen          #+#    #+#             */
/*   Updated: 2025/01/10 19:24:35 by ashaheen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	char			*str;
	unsigned char	letter;
	int				i;

	str = (char *)s;
	letter = (unsigned char)c;
	i = ft_strlen(s);
	if (letter == '\0')
		return ((char *)(s + i));
	while (i >= 0)
	{
		if (str[i] == letter)
			return ((char *)(s + i));
		i--;
	}
	return (NULL);
}
