/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymiyakos <ymiyakos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 13:42:58 by ymiyakos          #+#    #+#             */
/*   Updated: 2026/10/29 18:43:25 by ymiyakos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include <stdlib.h>

int	get_c_fd(int fd)
{
	static char	buf[BUFFER_SIZE];
	static char	*bufp;
	static int	n;

	if (n == 0)
	{
		n = read(fd, buf, BUFFER_SIZE);
		bufp = buf;
	}
	--n;
	if (n >= 0)
		return ((unsigned char)*bufp++);
	return (EOF);
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

char	*make_line(int fd)
{
	t_str	line;
	int		c;

	line.cap = BUFFER_SIZE;
	line.s = malloc(line.cap * sizeof(*(line.s)));
	if (!line.s)
		return (NULL);
	line.len = 0;
	while (1)
	{
		c = get_c_fd(fd);
		if (c == EOF)
			break ;
		if (line.len + 1 >= line.cap)
		{
			if (!realloc_line(&line))
				return (NULL);
		}
		line.s[line.len++] = (char)c;
		if (c == '\n')
			break ;
	}
	if (line.len == 0)
	{
		free(line.s);
		return (NULL);
	}
	line.s[line.len] = '\0';
	return (line.s);
}
