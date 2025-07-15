/**
 * @file lox.c
 * @brief
 *
 * Loxの実装
 *
 * @author Your Name
 *
 * @copyright Copyright (c) 2025 Your Name
*/

#include "lox.h"

#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


/**
 * @brief ファイルサイズの取得を行う関数
 * この関数を呼び出すことにより、シークが先頭に移動します。
 *
 * @param[in] fd サイズの取得を行うファイルのディスクリプタ
 *
 * @return 0以上=ファイルサイズ、負の数=エラー
 */
static long get_file_size(int fd)
{
	off_t file_size;
	off_t ret;

	if (fd < 0) {
		return -EINVAL;
	}

	/* シークをファイルの先頭に動かす */
	ret = lseek(fd, 0, SEEK_SET);
	if (ret < 0) {
		return -errno;
	}

	/* ファイルサイズを計算する */
	file_size = lseek(fd, 0, SEEK_END);
	if (file_size < 0) {
		return -errno;
	}

	/* シークをファイルの先頭に戻す */
	ret = lseek(fd, 0, SEEK_SET);
	if (ret < 0) {
		return -errno;
	}

	return file_size;
}

/**
 * @brief 指定されたファイルからデータを読み込む
 * lp_dataにはmallocした領域のアドレスが入るため、freeが必要
 * @param[out] lp_data データの書き込み先
 * @param[in] path 読み込むファイルのパス
 * @return 0=成功、負の数=エラー
 */
static int load_source_file(char **lp_data, const char *path)
{
	off_t file_size;
	char *tmp_data;
	ssize_t ret;
	int fd = -1;
	int ret_val = 0;

	if (lp_data == NULL || path == NULL) {
		return -EINVAL;
	}

	/* ソースファイルを開く */
	fd = open(path, O_RDONLY | FD_CLOEXEC);
	if (fd < 0) {
		return -errno;
	}

	/* ファイルサイズを取得する */
	file_size = get_file_size(fd);
	if (file_size < 0) {
		ret_val = (int)file_size;
		goto ERROR;
	}

	/* ファイルデータを格納する領域を確保する */
	tmp_data = (char *)malloc(sizeof(char) * (size_t)file_size);
	if (tmp_data == NULL) {
		ret_val = -errno;
		goto ERROR;
	}

	/* ファイルを読み込む */
	ret = read(fd, tmp_data, (size_t)file_size);
	if (ret != 0) {
		ret_val = -errno;
		goto ERROR;
	}

	*lp_data = tmp_data;

ERROR:
	if (fd >= 0) {
		ret = close(fd);
		if (ret != 0) {
			perror("Can't close file.\n");
		}
	}

	return ret_val;
}

static void report(int line, const char *where, const char *msg) {
	fprintf(stderr, "[line %d]Error %s: %s", line, where, msg);
}

static void error(int line, const char *msg)
{
	report(line, "", msg);
}

// TODO 最終的にはトークンを返す
// TODO スキャン用にファイルを分ける
// TODO Token用の構造体を作る
static int scan_source(const char *source)
{
	return 0;
}

/**
 * @brief loxを実行する関数
 *
 * @return 0=成功、負の数=エラー
 */
int run_lox_source_file(const char *path)
{
	int ret;
	char *source = NULL;
	int ret_val = 0;

	if (path == NULL) {
		return -EINVAL;
	}

	ret = load_source_file(&source, path);
	if (ret != 0) {
		ret_val = ret;
		frpintf(stderr, "[E %d]Cannot load file.\n", ret);
		goto ERROR;
	}

	ret = scan_source(source);

ERROR:
	if (source != NULL) {
		free(source);
	}

	return ret_val;
}
