/*
 * ファイル(引数なしなら標準入力)を get_next_line で全行読み、そのまま出力する。
 * run.sh が元ファイルと cmp で比較する。
 * あわせて「1行の形」が正しいかもチェックする。
 */
#include <fcntl.h>
#include <string.h>
#include "gnl.h"

int	main(int ac, char **av)
{
	int		fd;
	char	*line;
	size_t	n;
	int		prev_had_no_nl;

	fd = 0;
	if (ac > 1 && (fd = open(av[1], O_RDONLY)) < 0)
		return (perror(av[1]), 2);
	prev_had_no_nl = 0;
	while ((line = get_next_line(fd)))
	{
		n = strlen(line);
		if (prev_had_no_nl)
			return (fprintf(stderr, "a line came after a line without '\\n'\n"), 3);
		if (n == 0)
			return (fprintf(stderr, "empty string returned (should be NULL)\n"), 3);
		if (memchr(line, '\n', n) != &line[n - 1] && memchr(line, '\n', n))
			return (fprintf(stderr, "'\\n' in the middle of a line\n"), 3);
		prev_had_no_nl = (line[n - 1] != '\n');
		if (write(1, line, n) < 0)
			return (4);
		free(line);
	}
	line = get_next_line(fd);
	if (line)
		return (fprintf(stderr, "non-NULL after EOF: \"%s\"\n", line), 3);
	if (fd != 0)
		close(fd);
	return (0);
}
