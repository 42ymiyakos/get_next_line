*This project has been created as part of the 42 curriculum by ymiyakos.*

# get_next_line

## Description

- fd (ファイル記述子)がどのようなものか
- read 関数が何をできるか、その周辺の関数(open, close)
- c 言語の static 変数の仕組み

渡された fd から改行が来るまでの1行を読み取って返す関数

```c
char	*get_next_line(int fd);
```

#### 戻り値
- 読み取った1行(malloc された文字列。呼び出し側で free する)
- 行末の `\n` も返す文字列に含める
- ファイル末尾の行に `\n` がない場合も、その行を返す
- 読み終わり(EOF)・エラー時は NULL

### Bonus

#### 要件
- static 変数を一つのみで実装する
- 複数の fd を並列して処理できるようにする (static 変数は配列 `state[FD_MAX]` の1つだけで、fd ごとにその要素を1つ使う)

## Instructions

### Files

| ファイル | 役割 |
| --- | --- |
| `get_next_line.c` | 引数チェック、read 関数で読んだ文字を行に組み立て |
| `get_next_line_utils.c` | line の初期化、 line に文字を詰める、 line の長さを伸ばす |
| `get_next_line.h` | マクロ・構造体・プロトタイプの定義 |
| `get_next_line_bonus.c` / `_utils_bonus.c` / `_bonus.h` | 複数 fd 対応ver |

### Compilation

mandatory
```sh
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c main.c
```

bonus
```sh
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line_bonus.c get_next_line_utils_bonus.c main.c
```

### Usage

```c
#include <fcntl.h>
#include <stdio.h>
#include "get_next_line.h"

int	main(void)
{
	int		fd;
	char	*line;

	line = NULL;
	fd = open("text1.txt", O_RDONLY);
	if (fd == -1)
		return (1);
	while (1)
	{
		line = get_next_line(fd);
		if (line == NULL)
			break ;
		printf("%s", line);
		free(line);
	}
	close(fd);
	return (0);
}
```

## Algorithm

static 構造体の文字列ポインタを持つメンバに、指定された BUFFER_SIZE で malloc されたメモリ領域を指させ、 read にその領域を書き込み先として渡す
static 構造体のメンバなので、次の呼び出しでも内容が保存される

自作の `getc_fd` から1文字ずつ受け取り、 line に格納する。`\n` を格納した時点で、その行を返す。
次の1文字と終端の `\0` が line に収まらない場合(`len + 1 >= cap`)は、2倍の容量の line を新しく確保して中身をコピーし、古い line を解放する

read の戻り値(remain)が 0 の時は EOF 、-1 は READ_ERROR 。
`BUFFER_SIZE <= 0` や負の fd は、最初の引数チェック(`is_valid_args`)で弾いて NULL を返す。
buf も line も malloc でヒープに確保しているので、極端に大きい BUFFER_SIZE でも malloc が失敗して NULL を返すだけで、クラッシュしない(スタックに配列を置く実装と違い、スタックが溢れることはない)

read 用の buf は、EOF・エラーのときに free したあと NULL にする。
buf は static 変数のメンバなので次の呼び出しでも値が残る。NULL にしておかないと、`if (!state.buf)` の確保がスキップされ、解放済みの領域に read したり、同じ領域をもう一度 free (2重 free)したりしてしまう

line を作る際に strjoin を使う方法もある。ただし strjoin は文字列を足すたびに新しい領域を確保して、それまでの行全体をコピーし直すので、長い行ほどコピーの量が増えて遅くなる。
容量を2倍ずつ広げる方式なら、確保とコピーは容量が足りなくなったときだけで済み、その回数は行の長さに対して対数的にしか増えない。そのためこちらを選んだ。

## Resources

- `man 2 read`
- [sourceforge-read](https://linuxjm.sourceforge.io/html/LDP_man-pages/man2/read.2.html)
- [1時間で書くGet Next Line(GNL)](https://zenn.dev/grigri_grin/articles/bf45a9fa50f25f)
- [musl-getdelim](https://github.com/BlankOn/musl/blob/master/src/stdio/getdelim.c)
- [NetBSD-getdelim](https://github.com/NetBSD/src/blob/trunk/tools/compat/getline.c)
- [システムリソースの制限(ulimit)](https://qiita.com/hot_study_man/items/24e2bb953d4dca539c75)
- [Linuxで開ける最大のfile descriptorの数について調べたメモ](https://qiita.com/takc923/items/238597a53a2328025b09)
- [sysctl_nr_open定義](https://github.com/torvalds/linux/blob/master/fs/file.c#L100)
- [うさぎでもわかる計算機システム　Part13　4つのメモリ領域・システムコール](https://www.momoyama-usagi.com/entry/info-calc-sys13#gsc.tab=0)

### Use of AI

- 設計の相談
- 実装のレビュー・エッジケースの指摘
- README雛形の作成・添削

`test/`
- テスト設計相談
- テストの実装

## Technical choices

### FD_MAX = 1048576 (bonus)
- この環境で1プロセスが開ける fd の上限(`ulimit -n`)に合わせた。これは Linux の `fs.nr_open` のデフォルト値でもあり、`RLIMIT_NOFILE` はこれより大きくできない
- `#ifndef` で囲んでいるので、`-D FD_MAX=...` でコンパイル時に変更できる
- `t_fd_state` は 24 バイトなので、配列全体で約 24MB になる。static 変数なのでスタックではなく BSS に置かれ、スタックが溢れることはない。また起動時に 0 で初期化されるので、`buf == NULL` / `remain == 0` の初期状態を自分で作る必要がない
- fd ごとに連結リストを持つ方法もあるが、メモリを多めに使う代わりに、fd を添字にして O(1) で状態を取り出せる単純さを選んだ

### 1文字ずつ取り出す `getc_fd`
- 標準ライブラリの `getc` と同じ形にした(musl の `getdelim` を参考)
- 「read してバッファから1文字取り出す」処理と「行を組み立てる」処理が分かれるので、それぞれが単純になる
- バッファから `\n` を探してまとめて切り出す方法より関数呼び出しは増えるが、読みやすさを優先した

### 戻り値を `unsigned char` にキャストする
- `0xFF` のバイトを `char` のまま返すと -1 になり、`EOF`(-1) と区別できなくなる。`unsigned char` にキャストして 0〜255 で返すことで防いでいる
- エラーは `EOF`(-1)・`READ_ERROR`(-2)・`MALLOC_ERROR`(-3) と、文字と重ならない別々の負の値にして、呼び出し側で区別できるようにした

### line を2倍ずつ伸ばす
- strjoin で毎回つなげる方法と比べて、確保とコピーの回数が行の長さに対して対数的にしか増えない(詳細は Algorithm を参照)

### buf を必要になったときに確保し、EOF・エラーで解放する
- 最後まで読み切れば、buf はリークせずに解放される
- 制限: 呼び出し側が途中で読むのをやめると、その fd の buf は解放されずに残る。ただし static 変数から辿れるので、valgrind では "definitely lost" ではなく "still reachable" になる