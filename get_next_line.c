/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymiyakos <ymiyakos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 13:22:40 by ymiyakos          #+#    #+#             */
/*   Updated: 2026/10/31 18:51:46 by ymiyakos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

int	getc_fd(int fd)
{
	static t_fd_state	fd_state;

	if (fd_state.n == 0)
	{
		if (!fd_state.buf)
			fd_state.buf = malloc(BUFFER_SIZE * sizeof(*fd_state.buf));
		if (!fd_state.buf)
			return (MALLOC_ERROR);
		fd_state.n = read(fd, fd_state.buf, BUFFER_SIZE);
		if (fd_state.n <= 0)
		{
			free(fd_state.buf);
			fd_state.buf = NULL;
			if (fd_state.n == -1)
			{
				fd_state.n = 0;
				return (READ_ERROR);
			}
			return (EOF);
		}
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
		if (c == EOF || c == READ_ERROR || c == MALLOC_ERROR)
			break ;
		if (!line_push(&line, c))
			return (NULL);
		if (c == '\n')
			break ;
	}
	if (!line.len || c == READ_ERROR || c == MALLOC_ERROR)
	{
		free(line.s);
		return (NULL);
	}
	line.s[line.len] = '\0';
	return (line.s);
}
