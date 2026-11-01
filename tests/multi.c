/*
 * bonus 用: 複数の fd を1行ずつ順番に読み、fd ごとに別ファイルへ書き出す。
 * 使い方: ./multi <出力ディレクトリ> <入力ファイル>...
 * 出力は <出力ディレクトリ>/<番号>.out。run.sh が入力と比較する。
 */
#include <fcntl.h>
#include <string.h>
#include "gnl.h"

int	main(int ac, char **av)
{
	int		n;
	int		*in;
	int		*out;
	int		*done;
	int		remaining;
	int		i;
	char	*line;
	char	path[4096];

	if (ac < 3)
		return (2);
	n = ac - 2;
	in = malloc(n * sizeof(int));
	out = malloc(n * sizeof(int));
	done = calloc(n, sizeof(int));
	for (i = 0; i < n; i++)
	{
		in[i] = open(av[i + 2], O_RDONLY);
		snprintf(path, sizeof(path), "%s/%d.out", av[1], i);
		out[i] = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (in[i] < 0 || out[i] < 0)
			return (perror("open"), 2);
	}
	remaining = n;
	while (remaining > 0)
	{
		for (i = 0; i < n; i++)
		{
			if (done[i])
				continue ;
			line = get_next_line(in[i]);
			if (!line)
			{
				done[i] = 1;
				remaining--;
				continue ;
			}
			if (write(out[i], line, strlen(line)) < 0)
				return (2);
			free(line);
		}
	}
	for (i = 0; i < n; i++)
	{
		close(in[i]);
		close(out[i]);
	}
	free(in);
	free(out);
	free(done);
	return (0);
}
