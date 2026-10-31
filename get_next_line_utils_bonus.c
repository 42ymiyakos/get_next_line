/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils_bonus.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymiyakos <ymiyakos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 13:42:58 by ymiyakos          #+#    #+#             */
/*   Updated: 2026/10/31 16:42:26 by ymiyakos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

int	is_valid_args(int fd)
{
	char	dummy[1];

	if (BUFFER_SIZE <= 0)
		return (0);
	if (fd >= FD_MAX)
		return (0);
	if (fd < 0)
		return (0);
	if (read(fd, dummy, 0) < 0)
		return (0);
	return (1);
}

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

int	line_init(t_line *line)
{
	line->cap = BUFFER_SIZE;
	line->len = 0;
	line->s = malloc(line->cap * sizeof(*(line->s)));
	if (!line->s)
		return (0);
	return (1);
}

int	line_realloc(t_line *line)
{
	char	*new_line;

	new_line = malloc((line->cap * 2) * sizeof(*new_line));
	if (!new_line)
	{
		free(line->s);
		return (0);
	}
	line->cap *= 2;
	ft_memcpy(new_line, line->s, line->len);
	free(line->s);
	line->s = new_line;
	return (1);
}

int	line_push(t_line *line, int c)
{
	if (line->len + 1 >= line->cap)
	{
		if (!line_realloc(line))
			return (0);
	}
	line->s[line->len++] = (char)c;
	return (1);
}
