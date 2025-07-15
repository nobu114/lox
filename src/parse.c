/**
 * @file parse.c
 * @brief
 *
 * Description
 *
 * @author Your Name
 *
 * @copyright Copyright (c) 2025 Your Name
*/

#include "app.h"
#include "parse.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief コマンドラインからの入力をパースする関数
 *
 * @param[out] cmdline コマンドラインの情報を格納する構造体
 * @param[in] argc 引数の数
 * @param[in,out] argv 引数の文字列
 *
 * @return 0=成功、負の数=エラー
 */
int app_parse_cmdline(struct app_info *info, int argc, char *argv[])
{
	int c;

	enum OPTION_VALS {
		HELP = 129,
	};

	/* 使用できるオプションの一覧 */
	struct option long_options[] = {
		{"help", no_argument, NULL, HELP},
	};

	/* オプションのパースを行う */
	for (; ; ) {
		c = getopt_long(argc, argv, "h", long_options, NULL);

		if (c == -1) {
			break;
		}

		switch (c) {
		case 'h':
		case HELP:
			/* helpを表示する場合は他に何もしない */
			info->cmdline.operation = OP_HELP;
			return 0;

		default:
			break;
		}
	}

	if (argc == 2) {
		/* ソースファイルのパスのみを指定された場合にインタプリタを起動する */
		if (info->cmdline.operation != OP_UNDEF) {
			/* すでに他の動作を指定された場合は何もしない */
			return 0;
		}

		info->cmdline.operation = OP_RUN_FILE;
		info->cmdline.path = argv[1];
	}

	return 0;
}
