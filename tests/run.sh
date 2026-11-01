#!/usr/bin/env bash
# get_next_line テスト
# 使い方:
#   tests/run.sh              mandatory と bonus の両方
#   tests/run.sh mandatory    mandatory だけ
#   tests/run.sh bonus        bonus だけ
#   SIZES="1 42" tests/run.sh BUFFER_SIZE を絞る
#   NO_VALGRIND=1 tests/run.sh  valgrind を省略

set -u
cd "$(dirname "$0")" || exit 1

ROOT=..
WORK=.work
F=$WORK/files
CC=${CC:-cc}
CFLAGS="-Wall -Wextra -Werror -g"
SIZES=${SIZES:-"1 2 3 4 5 8 42 1000 10000000"}
VG_SIZES="1 42"
VG="valgrind -q --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --error-exitcode=42"
TARGET=${1:-all}

if [ -t 1 ]; then R=$'\e[31m'; G=$'\e[32m'; Y=$'\e[33m'; N=$'\e[0m'; else R= G= Y= N=; fi
pass=0
fail=0
ok() { pass=$((pass + 1)); }
ng() { fail=$((fail + 1)); echo "  ${R}FAIL${N} $*"; }

if [ -n "${NO_VALGRIND:-}" ] || ! command -v valgrind >/dev/null; then
	VG_SIZES=""
fi

# ---------- テスト用ファイルを作る ----------
gen_files() {
	rm -rf "$F"
	mkdir -p "$F"
	: >"$F/empty"
	printf '\n' >"$F/nl_only"
	printf '\n\n\n\n\n' >"$F/nl_many"
	printf 'a' >"$F/one_char"
	printf 'abc' >"$F/no_trailing_nl"
	printf 'hello\nworld\n42 tokyo\n' >"$F/normal"
	printf 'hello\nworld\n42 tokyo' >"$F/normal_no_trailing_nl"
	printf '\n\nabc\n\n\ndef\n\n' >"$F/nl_mixed"
	printf '\xff\xfe\x80abc\n\xff\n' >"$F/high_bytes"
	{ head -c 100000 /dev/zero | tr '\0' 'a'; printf '\nend'; } >"$F/long_line"
	seq 1 20000 >"$F/many_lines"
	# 行の長さが BUFFER_SIZE の前後になるファイル
	for L in 1 2 3 4 5 7 8 9 41 42 43 999 1000 1001; do
		line=$(head -c "$L" /dev/zero | tr '\0' 'x')
		printf '%s\n%s\n%s\n' "$line" "$line" "$line" >"$F/len$L"
		printf '%s\n%s' "$line" "$line" >"$F/len${L}_no_trailing_nl"
	done
	cp "$ROOT"/text*.txt "$F/" 2>/dev/null
	# malloc 失敗テスト用 (line_grow が何回か起きる長さ)
	{ head -c 300 /dev/zero | tr '\0' 'x'; printf '\na\n\nb'; } >"$WORK/malloc_input"
}

# build <出力> <テストのソース> [追加フラグ...]   (BS, SRCS, DEF を使う)
build() {
	local out=$1 src=$2
	shift 2
	$CC $CFLAGS -D BUFFER_SIZE="$BS" $DEF -I"$ROOT" -I. "$@" -o "$out" "$src" $SRCS 2>"$out.log"
}

build_failed() {
	ng "$1: build failed ($2)"
	sed 's/^/      /' "$2" | head -5
}

# ---------- 1つの BUFFER_SIZE でのテスト ----------
test_size() {
	local B=$WORK/bin/$VARIANT/$BS f name before=$fail
	mkdir -p "$B"
	local tag="[$VARIANT BUFFER_SIZE=$BS]"

	if ! build "$B/cat" gnl_cat.c; then build_failed "$tag" "$B/cat.log"; return; fi
	build "$B/edge" edge.c || { build_failed "$tag" "$B/edge.log"; return; }

	# ファイル全体を読んで元と一致するか
	for f in "$F"/*; do
		name=$(basename "$f")
		if ! "$B/cat" "$f" >"$B/out" 2>"$B/err"; then
			ng "$tag $name: $(head -1 "$B/err") (exit $?)"
		elif ! cmp -s "$f" "$B/out"; then
			ng "$tag $name: output differs from file"
		else ok; fi
	done

	# 標準入力 (リダイレクトとパイプ)
	for name in normal no_trailing_nl long_line many_lines; do
		if "$B/cat" <"$F/$name" | cmp -s - "$F/$name"; then ok; else ng "$tag stdin < $name"; fi
		if cat "$F/$name" | "$B/cat" | cmp -s - "$F/$name"; then ok; else ng "$tag pipe | $name"; fi
	done
	# read が少しずつしか返ってこない入力
	if (printf 'ab'; sleep 0.1; printf 'c\nd'; sleep 0.1; printf 'e\n') | "$B/cat" |
		cmp -s - <(printf 'abc\nde\n'); then ok; else ng "$tag slow pipe"; fi

	# 境界ケース
	if "$B/edge" "$WORK" >"$B/edge.out" 2>&1; then ok; else
		ng "$tag edge cases"
		sed 's/^/    /' "$B/edge.out"
	fi

	# 読みかけの途中で read が失敗する
	if build "$B/read_error" read_error.c -Wl,--wrap=read; then
		local rcmd="$B/read_error"
		[[ " $VG_SIZES " == *" $BS "* ]] && rcmd="$VG $B/read_error"
		if $rcmd "$WORK" >"$B/re.out" 2>"$B/vg"; then ok; else
			ng "$tag read error in the middle"
			sed 's/^/    /' "$B/re.out" "$B/vg" | head -15
		fi
	else
		build_failed "$tag" "$B/read_error.log"
	fi

	# bonus: 複数 fd を同時に
	if [ "$VARIANT" = bonus ]; then
		if build "$B/multi" multi.c; then
			local files=("$F/long_line" "$F/many_lines" "$F/normal_no_trailing_nl" "$F/empty" "$F/nl_many" "$F/len42")
			# 200 個の fd を同時に開く
			local many=() i
			for i in $(seq 1 200); do many+=("${files[$((i % ${#files[@]}))]}"); done
			rm -rf "$B/multi_out" && mkdir -p "$B/multi_out"
			if "$B/multi" "$B/multi_out" "${many[@]}"; then
				local bad=0
				for i in "${!many[@]}"; do
					cmp -s "${many[$i]}" "$B/multi_out/$i.out" || bad=$((bad + 1))
				done
				if [ $bad -eq 0 ]; then ok; else ng "$tag multi fd: $bad / ${#many[@]} files differ"; fi
			else
				ng "$tag multi fd: crashed (exit $?)"
			fi
		else
			build_failed "$tag" "$B/multi.log"
		fi
	fi

	# valgrind (リーク・不正アクセス) と malloc 失敗
	if [[ " $VG_SIZES " == *" $BS "* ]]; then
		for f in "$F"/*; do
			name=$(basename "$f")
			[ "$name" = many_lines ] && continue # 遅いので省略
			if $VG "$B/cat" "$f" >/dev/null 2>"$B/vg"; then ok; else
				ng "$tag valgrind cat $name"
				sed 's/^/      /' "$B/vg" | head -15
			fi
		done
		if $VG "$B/edge" "$WORK" >/dev/null 2>"$B/vg"; then ok; else
			ng "$tag valgrind edge"
			sed 's/^/      /' "$B/vg" | head -15
		fi
		if build "$B/malloc_fail" malloc_fail.c -Wl,--wrap=malloc; then
			if $VG "$B/malloc_fail" "$WORK/malloc_input" >"$B/mf.out" 2>"$B/vg"; then
				ok
			else
				ng "$tag malloc failure (exit $?)"
				sed 's/^/      /' "$B/vg" | head -15
			fi
		else
			build_failed "$tag" "$B/malloc_fail.log"
		fi
	fi

	if [ $fail -eq $before ]; then echo "  ${G}OK${N}   $tag"; fi
}

# BUFFER_SIZE が 0 や負のとき: ビルドでき、NULL を返すか
test_bad_size() {
	local B=$WORK/bin/$VARIANT/$BS
	mkdir -p "$B"
	local tag="[$VARIANT BUFFER_SIZE=$BS]"
	if ! build "$B/edge" edge.c; then build_failed "$tag" "$B/edge.log"; return; fi
	if "$B/edge" "$WORK" >"$B/edge.out" 2>&1; then
		ok
		echo "  ${G}OK${N}   $tag"
	else
		ng "$tag should return NULL"
		sed 's/^/    /' "$B/edge.out"
	fi
}

run_variant() {
	VARIANT=$1
	if [ "$VARIANT" = bonus ]; then
		SRCS="$ROOT/get_next_line_bonus.c $ROOT/get_next_line_utils_bonus.c"
		DEF="-DBONUS"
	else
		SRCS="$ROOT/get_next_line.c $ROOT/get_next_line_utils.c"
		DEF=""
	fi
	echo "${Y}== $VARIANT ==${N}"
	for BS in $SIZES; do test_size; done
	for BS in 0 -1; do test_bad_size; done
}

mkdir -p "$WORK"
gen_files
[ -z "$VG_SIZES" ] && echo "${Y}(valgrind skipped)${N}"
case $TARGET in
mandatory) run_variant mandatory ;;
bonus) run_variant bonus ;;
all) run_variant mandatory && run_variant bonus ;;
*) echo "usage: $0 [mandatory|bonus|all]" && exit 2 ;;
esac

echo
echo "passed: ${G}$pass${N}  failed: ${R}$fail${N}"
[ $fail -eq 0 ]
