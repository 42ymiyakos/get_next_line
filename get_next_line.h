/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymiyakos <ymiyakos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 13:41:25 by ymiyakos          #+#    #+#             */
/*   Updated: 2026/11/01 11:06:00 by ymiyakos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif

# define READ_ERROR -2
# define MALLOC_ERROR -3

typedef struct s_fd_state
{
	char	*buf;
	char	*pos;
	ssize_t	remain;
}	t_fd_state;

typedef struct s_line
{
	size_t	cap;
	size_t	len;
	char	*s;
}	t_line;

char	*get_next_line(int fd);

// utils
int		is_valid_args(int fd);
int		line_init(t_line *line);
int		line_push(t_line *line, int c);

#endif