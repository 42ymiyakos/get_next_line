/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymiyakos <ymiyakos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 13:22:40 by ymiyakos          #+#    #+#             */
/*   Updated: 2026/11/01 12:02:27 by ymiyakos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

static int	is_valid_args(int fd)
{
	if (BUFFER_SIZE <= 0)
		return (0);
	if (fd >= FD_MAX)
		return (0);
	if (fd < 0)
		return (0);
	return (1);
}

static int	getc_fd(int fd)
{
	static t_fd_state	state[FD_MAX];

	if (state[fd].remain == 0)
	{
		if (!state[fd].buf)
			state[fd].buf = malloc(BUFFER_SIZE * sizeof(*state[fd].buf));
		if (!state[fd].buf)
			return (MALLOC_ERROR);
		state[fd].remain = read(fd, state[fd].buf, BUFFER_SIZE);
		if (state[fd].remain <= 0)
		{
			free(state[fd].buf);
			state[fd].buf = NULL;
			if (state[fd].remain == -1)
			{
				state[fd].remain = 0;
				return (READ_ERROR);
			}
			return (EOF);
		}
		state[fd].pos = state[fd].buf;
	}
	--state[fd].remain;
	return ((unsigned char)*state[fd].pos++);
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
