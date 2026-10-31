/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymiyakos <ymiyakos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 13:22:40 by ymiyakos          #+#    #+#             */
/*   Updated: 2026/10/31 16:10:19 by ymiyakos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

int	getc_fd(int fd)
{
	static t_fd_state	fd_state;

	if (fd_state.n == 0)
	{
		fd_state.n = read(fd, fd_state.buf, BUFFER_SIZE);
		if (fd_state.n == -1)
		{
			fd_state.n = 0;
			return (READ_ERROR);
		}
		if (fd_state.n == 0)
			return (EOF);
		fd_state.bufp = fd_state.buf;
	}
	--fd_state.n;
	return ((unsigned char)*fd_state.bufp++);
}

char	*get_next_line(int fd)
{
	t_line	line;
	int		c;

	if (!is_valid_args(fd) || !line_init(&line))
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
