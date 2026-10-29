/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymiyakos <ymiyakos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 13:42:58 by ymiyakos          #+#    #+#             */
/*   Updated: 2026/10/29 19:28:44 by ymiyakos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	unsigned char		*dest_p;
	const unsigned char	*src_p;
	size_t				i;

	dest_p = (unsigned char *)dest;
	src_p = (const unsigned char *)src;
	i = 0;
	while (i < n)
	{
		dest_p[i] = src_p[i];
		++i;
	}
	return (dest);
}

int	realloc_line(t_str *line)
{
	char	*new_line;

	new_line = malloc((line->cap * 2) * sizeof(*new_line));
	if (!new_line)
	{
		free(line->s);
		return (0);
	}
	ft_memcpy(new_line, line->s, line->len);
	line->cap *= 2;
	free(line->s);
	line->s = new_line;
	return (1);
}
