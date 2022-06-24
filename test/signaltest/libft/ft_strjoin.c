/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wchae <wchae@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/12 16:32:03 by wchae             #+#    #+#             */
/*   Updated: 2022/06/12 16:33:46 by wchae            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*rt;
	int		i;
	int		j;

	j = 0;
	i = 0;
	if (!s1 || !s2)
		return (0);
	rt = malloc(ft_strlen(s1) + ft_strlen(s2) + 1);
	if (!rt)
		return (0);
	while (s1[i])
	{
		rt[i] = s1[i];
		i++;
	}
	while (s2[j])
		rt[i++] = s2[j++];
	rt[i] = 0;
	s1 = 0;
	return (rt);
}
