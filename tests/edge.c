/*
 * 境界ケースを1つずつ確認する。失敗したら FAIL を表示して終了コード1。
 * 使い方: ./edge <作業用ディレクトリ>
 */
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include "gnl.h"

static int	g_fail = 0;
static char	*g_dir;

static void	show(const char *s)
{
	if (!s)
	{
		printf("NULL");
		return ;
	}
	putchar('"');
	for (; *s; s++)
	{
		if (*s == '\n')
			printf("\\n");
		else if ((unsigned char)*s < 32 || (unsigned char)*s >= 127)
			printf("\\x%02x", (unsigned char)*s);
		else
			putchar(*s);
	}
	putchar('"');
}

/* got は free する */
static void	expect(const char *name, char *got, const char *want)
{
	int	ok;

	ok = (!got && !want) || (got && want && strcmp(got, want) == 0);
	if (!ok)
	{
		g_fail++;
		printf("  FAIL %s: want ", name);
		show(want);
		printf(", got ");
		show(got);
		printf("\n");
	}
	free(got);
}

static char	*path_of(const char *name)
{
	static char	path[4096];

	snprintf(path, sizeof(path), "%s/%s", g_dir, name);
	return (path);
}

/* content を書いたファイルを作り、読み込み用に開いて返す */
static int	open_with(const char *name, const char *content)
{
	int	fd;

	fd = open(path_of(name), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0 || write(fd, content, strlen(content)) < 0)
		exit((perror(name), 2));
	close(fd);
	fd = open(path_of(name), O_RDONLY);
	if (fd < 0)
		exit((perror(name), 2));
	return (fd);
}

#if BUFFER_SIZE <= 0

int	main(int ac, char **av)
{
	int	fd;

	if (ac < 2)
		return (2);
	g_dir = av[1];
	fd = open_with("e_bs0", "abc\n");
	expect("BUFFER_SIZE<=0 -> NULL", get_next_line(fd), NULL);
	close(fd);
	return (g_fail != 0);
}

#else

int	main(int ac, char **av)
{
	int	fd;
	int	fd2;

	if (ac < 2)
		return (2);
	g_dir = av[1];

	/* 不正な fd */
	expect("fd -1", get_next_line(-1), NULL);
	expect("fd INT_MIN", get_next_line(INT_MIN), NULL);
	expect("unopened fd 4242", get_next_line(4242), NULL);
	fd = open_with("e_closed", "abc\n");
	close(fd);
	expect("closed fd", get_next_line(fd), NULL);
	fd = open(path_of("e_closed"), O_WRONLY);
	expect("write-only fd", get_next_line(fd), NULL);
	close(fd);
	fd = open(g_dir, O_RDONLY);
	expect("directory fd", get_next_line(fd), NULL);
	close(fd);

	/* 中身の境界ケース */
	fd = open_with("e_empty", "");
	expect("empty #1", get_next_line(fd), NULL);
	expect("empty #2 (call again)", get_next_line(fd), NULL);
	close(fd);

	fd = open_with("e_nl", "\n");
	expect("only \\n #1", get_next_line(fd), "\n");
	expect("only \\n #2", get_next_line(fd), NULL);
	expect("only \\n #3", get_next_line(fd), NULL);
	close(fd);

	fd = open_with("e_nonl", "abc");
	expect("no trailing \\n #1", get_next_line(fd), "abc");
	expect("no trailing \\n #2", get_next_line(fd), NULL);
	close(fd);

	fd = open_with("e_mixed", "a\nb\n\nc");
	expect("mixed #1", get_next_line(fd), "a\n");
	expect("mixed #2", get_next_line(fd), "b\n");
	expect("mixed #3", get_next_line(fd), "\n");
	expect("mixed #4", get_next_line(fd), "c");
	expect("mixed #5", get_next_line(fd), NULL);
	close(fd);

	/* 0xff は (char) のままだと -1 (= EOF) と区別できなくなる */
	fd = open_with("e_high", "\xff\xfe\n\xff");
	expect("byte 0xff #1", get_next_line(fd), "\xff\xfe\n");
	expect("byte 0xff #2", get_next_line(fd), "\xff");
	expect("byte 0xff #3", get_next_line(fd), NULL);
	close(fd);

	/* 不正な呼び出しを挟んでも、読みかけの fd の状態が壊れない */
	fd = open_with("e_between", "a1\na2\na3\n");
	expect("between #1", get_next_line(fd), "a1\n");
	expect("between: fd -1", get_next_line(-1), NULL);
	expect("between: fd 4242", get_next_line(4242), NULL);
	expect("between #2", get_next_line(fd), "a2\n");
	expect("between #3", get_next_line(fd), "a3\n");
	expect("between #4", get_next_line(fd), NULL);
	close(fd);

	/* 同じ fd 番号を EOF 後に別ファイルで再利用 */
	fd = open_with("e_reuse1", "first\n");
	expect("reuse: file1 #1", get_next_line(fd), "first\n");
	expect("reuse: file1 #2", get_next_line(fd), NULL);
	close(fd);
	fd = open_with("e_reuse2", "second\n");
	expect("reuse: file2 #1", get_next_line(fd), "second\n");
	expect("reuse: file2 #2", get_next_line(fd), NULL);
	close(fd);

# ifdef BONUS
	expect("fd FD_MAX", get_next_line(FD_MAX), NULL);
	expect("fd INT_MAX", get_next_line(INT_MAX), NULL);

	/* 2つの fd を交互に読む */
	fd = open_with("e_a", "a1\na2\na3");
	fd2 = open_with("e_b", "b1\nb2\n");
	expect("interleave a1", get_next_line(fd), "a1\n");
	expect("interleave b1", get_next_line(fd2), "b1\n");
	expect("interleave a2", get_next_line(fd), "a2\n");
	expect("interleave fd -1", get_next_line(-1), NULL);
	expect("interleave b2", get_next_line(fd2), "b2\n");
	expect("interleave a3", get_next_line(fd), "a3");
	expect("interleave b EOF", get_next_line(fd2), NULL);
	expect("interleave a EOF", get_next_line(fd), NULL);
	close(fd);
	close(fd2);
# else
	(void)fd2;
# endif
	return (g_fail != 0);
}

#endif
