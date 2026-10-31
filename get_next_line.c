/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymiyakos <ymiyakos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 13:22:40 by ymiyakos          #+#    #+#             */
/*   Updated: 2026/10/31 13:02:53 by ymiyakos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

int	getc_fd(int fd)
{
	static char	buf[BUFFER_SIZE];
	static char	*bufp;
	static int	n;

	if (n == 0)
	{
		n = read(fd, buf, BUFFER_SIZE);
		if (n == -1)
		{
			n = 0;
			return (READ_ERROR);
		}
		if (n == 0)
			return (EOF);
		bufp = buf;
	}
	--n;
	return ((unsigned char)*bufp++);
}

char	*get_next_line(int fd)
{
	t_line	line;
	int		c;

	if (fd < 0 || BUFFER_SIZE <= 0 || !line_init(&line))
		return (NULL);
	while (1)
	{
		c = getc_fd(fd);
		if (c == READ_ERROR || c == EOF)
			break ;
		if (!line_push(&line, c))
			return (NULL);
		if (c == '\n')
			break ;
	}
	if (!line.len || c == READ_ERROR)
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
// 	printf("%s", get_next_line(fd));
// 	printf("%s", get_next_line(fd));
// 	printf("%s", get_next_line(fd));
// 	printf("%s", get_next_line(fd));
// 	close(fd);
// 	return (0);
// }
