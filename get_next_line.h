/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymiyakos <ymiyakos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 13:41:25 by ymiyakos          #+#    #+#             */
/*   Updated: 2026/10/31 12:52:50 by ymiyakos         ###   ########.fr       */
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

typedef struct s_str
{
	size_t	cap;
	size_t	len;
	char	*s;
}	t_line;

char	*get_next_line(int fd);
int		getc_fd(int fd);

// utils
void	*ft_memcpy(void *dest, const void *src, size_t n);
int		line_init(t_line *line);
int		line_realloc(t_line *line);
int		line_push(t_line *line, int c);

#endif