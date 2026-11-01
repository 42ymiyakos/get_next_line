/*
 * 読みかけの途中で read が -1 を返したときの動作を確認する。
 * -Wl,--wrap=read を付けてリンクし、g_read_fails が立っている間は read を失敗させる。
 * 期待: エラー時は NULL を返し、その fd の残りバッファは捨てられる。
 *       開き直した同じ fd 番号は、最初から読める。
 * 使い方: ./read_error <作業用ディレクトリ>
 */
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include "gnl.h"

ssize_t	__real_read(int fd, void *buf, size_t n);

static int	g_read_fails = 0;
static int	g_fail = 0;

ssize_t	__wrap_read(int fd, void *buf, size_t n)
{
	if (g_read_fails)
	{
		errno = EIO;
		return (-1);
	}
	return (__real_read(fd, buf, n));
}

static void	expect(int no, char *got, const char *want)
{
	if (!((!got && !want) || (got && want && strcmp(got, want) == 0)))
	{
		g_fail++;
		printf("  FAIL read error #%d: want %s%s%s, got %s%s%s\n", no,
			want ? "\"" : "", want ? want : "NULL", want ? "\"" : "",
			got ? "\"" : "", got ? got : "NULL", got ? "\"" : "");
	}
	free(got);
}

int	main(int ac, char **av)
{
	char		path[4096];
	const char	*content = "aaaaaaaaaa\nbbbbbbbbbb\ncccccccccc\ndddddddddd\n";
	int			fd;
	char		*line;

	if (ac < 2)
		return (2);
	snprintf(path, sizeof(path), "%s/read_error.txt", av[1]);
	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0 || write(fd, content, strlen(content)) < 0)
		return (2);
	close(fd);

	fd = open(path, O_RDONLY);
	expect(1, get_next_line(fd), "aaaaaaaaaa\n");
	expect(2, get_next_line(fd), "bbbbbbbbbb\n");
	g_read_fails = 1;
	/* 残りのデータがバッファに入っていても、エラー中は最後には NULL になるはず */
	while ((line = get_next_line(fd)))
		free(line);
	expect(3, get_next_line(fd), NULL);
	g_read_fails = 0;
	close(fd);

	fd = open(path, O_RDONLY);
	expect(4, get_next_line(fd), "aaaaaaaaaa\n");
	expect(5, get_next_line(fd), "bbbbbbbbbb\n");
	expect(6, get_next_line(fd), "cccccccccc\n");
	expect(7, get_next_line(fd), "dddddddddd\n");
	expect(8, get_next_line(fd), NULL);
	close(fd);
	return (g_fail != 0);
}
