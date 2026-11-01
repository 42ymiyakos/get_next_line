/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymiyakos <ymiyakos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 13:22:40 by ymiyakos          #+#    #+#             */
/*   Updated: 2026/11/01 11:33:03 by ymiyakos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static int	is_valid_args(int fd)
{
	char	dummy[1];

	if (BUFFER_SIZE <= 0)
		return (0);
	if (fd < 0)
		return (0);
	if (read(fd, dummy, 0) < 0)
		return (0);
	return (1);
}

static int	getc_fd(int fd)
{
	static t_fd_state	state;

	if (state.remain == 0)
	{
		if (!state.buf)
			state.buf = malloc(BUFFER_SIZE * sizeof(*state.buf));
		if (!state.buf)
			return (MALLOC_ERROR);
		state.remain = read(fd, state.buf, BUFFER_SIZE);
		if (state.remain <= 0)
		{
			free(state.buf);
			state.buf = NULL;
			if (state.remain == -1)
			{
				state.remain = 0;
				return (READ_ERROR);
			}
			return (EOF);
		}
		state.pos = state.buf;
	}
	--state.remain;
	return ((unsigned char)*state.pos++);
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
