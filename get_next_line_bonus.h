/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.h                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymiyakos <ymiyakos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 13:41:25 by ymiyakos          #+#    #+#             */
/*   Updated: 2026/10/31 18:48:43 by ymiyakos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_BONUS_H
# define GET_NEXT_LINE_BONUS_H

# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif

# define FD_MAX 1048576
# define READ_ERROR -2
# define MALLOC_ERROR -3

typedef struct s_fd_state
{
	char	*buf;
	char	*bufp;
	int		n;
}	t_fd_state;

typedef struct s_str
{
	size_t	cap;
	size_t	len;
	char	*s;
}	t_line;

char	*get_next_line(int fd);
int		getc_fd(int fd);

// utils
int		is_valid_args(int fd);
void	*ft_memcpy(void *dest, const void *src, size_t n);
int		line_init(t_line *line);
int		line_realloc(t_line *line);
int		line_push(t_line *line, int c);

#endif