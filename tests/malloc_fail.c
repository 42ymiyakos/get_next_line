/*
 * malloc を「N 回目だけ NULL を返す」ものに差し替えて、N = 1, 2, 3... と全部試す。
 * リンク時に -Wl,--wrap=malloc を付けると、gnl の malloc 呼び出しが
 * __wrap_malloc に、__real_malloc が本物の malloc につながる。
 * クラッシュしないこと、valgrind でリークが出ないことを確認する。
 * 使い方: ./malloc_fail <入力ファイル>
 */
#include <fcntl.h>
#include "gnl.h"

void	*__real_malloc(size_t size);

static int	g_armed = 0;
static long	g_count = 0;
static long	g_fail_at = 0;

void	*__wrap_malloc(size_t size)
{
	if (g_armed && ++g_count == g_fail_at)
		return (NULL);
	return (__real_malloc(size));
}

/* fail_at 回目の malloc を失敗させながら最後まで読む。malloc の回数を返す */
static long	run(const char *path, long fail_at)
{
	int		fd;
	char	*line;
	long	count;

	fd = open(path, O_RDONLY);
	if (fd < 0)
		exit((perror(path), 2));
	g_count = 0;
	g_fail_at = fail_at;
	g_armed = 1;
	while ((line = get_next_line(fd)))
		free(line);
	g_armed = 0;
	count = g_count;
	/* 失敗で途中終了した残りを読み切り、static なバッファも解放させる */
	while ((line = get_next_line(fd)))
		free(line);
	close(fd);
	return (count);
}

int	main(int ac, char **av)
{
	long	total;
	long	i;

	if (ac < 2)
		return (2);
	total = run(av[1], 0);
	for (i = 1; i <= total; i++)
		run(av[1], i);
	printf("    malloc failure: tried each of %ld malloc calls\n", total);
	return (0);
}
