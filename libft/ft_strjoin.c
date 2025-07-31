/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashaheen <ashaheen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 19:14:53 by ashaheen          #+#    #+#             */
/*   Updated: 2025/01/11 11:49:39 by ashaheen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*join;
	size_t	len1;
	size_t	len2;

	if (!s1 || !s2)
		return (NULL);
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	join = (char *)malloc(sizeof(char) * (len1 + len2 + 1));
	if (!join)
		return (NULL);
	len1 = 0;
	len2 = 0;
	while (s1[len1])
	{
		join[len1] = s1[len1];
		len1++;
	}
	while (s2[len2])
	{
		join[len1++] = s2[len2];
		len2++;
	}
	join[len1] = '\0';
	return ((char *)join);
}
