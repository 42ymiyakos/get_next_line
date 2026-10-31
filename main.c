#include <fcntl.h>
#include "get_next_line_bonus.h"

int	main(void)
{
	char	*files[3] = {"text1.txt", "text2.txt", "text3.txt"};
	int		fds[3];
	int		done[3] = {0, 0, 0};
	int		remaining;
	char	*line;
	int		i;

	for (i = 0; i < 3; i++)
	{
		fds[i] = open(files[i], O_RDONLY);
		if (fds[i] == -1)
			return (perror(files[i]), 1);
	}
	remaining = 3;
	while (remaining > 0)
	{
		for (i = 0; i < 3; i++)
		{
			if (done[i])
				continue ;
			line = get_next_line(fds[i]);
			if (!line)
			{
				printf("[%s] (EOF)\n", files[i]);
				done[i] = 1;
				remaining--;
				continue ;
			}
			printf("[%s] %s", files[i], line);
			free(line);
		}
	}
	for (i = 0; i < 3; i++)
		close(fds[i]);
	return (0);
}

