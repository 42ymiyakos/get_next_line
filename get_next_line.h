/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymiyakos <ymiyakos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 13:41:25 by ymiyakos          #+#    #+#             */
/*   Updated: 2026/10/29 18:53:09 by ymiyakos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <unistd.h>
# include <stdio.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif

typedef struct s_str
{
	char	*s;
	size_t	len;
	size_t	cap;
}	t_str;

char	*get_next_line(int fd);

// utils
int		get_c_fd(int fd);
int		realloc_line(t_str *line);
char	*make_line(int fd);

// libft
void	*ft_memcpy(void *dest, const void *src, size_t n);

#endif