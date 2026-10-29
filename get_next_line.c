/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymiyakos <ymiyakos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 13:22:40 by ymiyakos          #+#    #+#             */
/*   Updated: 2026/10/29 19:35:17 by ymiyakos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

int	get_c_fd(int fd)
{
	static char	buf[BUFFER_SIZE];
	static char	*bufp;
	static int	n;

	if (n == 0)
	{
		n = read(fd, buf, BUFFER_SIZE);
		if (n == -1)
			return (NULL);
		if (n == 0)
		{
			n = 0;
			return (EOF);
		}
		bufp = buf;
	}
	--n;
	return ((unsigned char)*bufp++);
}

char	*get_next_line(int fd)
{
	t_str	line;
	int		c;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
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

// #include <fcntl.h>

// int	main(void)
// {
// 	int		fd;

// 	fd = open("text.txt", O_RDONLY);
// 	if (fd == -1)
// 		return (1);
// 	printf("%s", get_next_line(fd));
// 	close(fd);
// 	return (0);
// }
