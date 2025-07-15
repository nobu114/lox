/**
 * @file main.c
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
#include "lox.h"

#include <stdio.h>
#include <string.h>

#define USAGE "\
Usage:\n\
  loxc [options]\n\
\n\
OPTIONS\n\
  -h, --help      show list of command-line options.\n\
"


/**
 * @brief app_infoの初期化を行う関数
 *
 * @param[out] info 初期化を行う構造体
 *
 * @return
 * 0=成功、
 * -EINVAL=引数がNULL
 */
static int app_init_app_info(struct app_info *info)
{
	memset(info, 0x00, sizeof(struct app_info));

	return 0;
}

/**
 * @brief アプリケーションの終了処理を行う関数
 * @param[out] info アプリケーションの情報を格納する構造体
 * @return 0=成功、負の数=エラー
 */
static int deinit_app(struct app_info *info)
{
	return 0;
}

/**
 * @brief アプリケーションの初期化を行う関数
 * @param[out] info 初期化を行うアプリケーションの情報を格納する構造体
 * @return 0=成功、負の数=エラー
 */
static int init_app(struct app_info *info)
{
	return 0;
}

/**
 * @brief アプリケーションのメイン処理
 * @param[in] info アプリケーションの情報
 * @return 0=成功、負の数=エラー
 */
static int main_proc(struct app_info *info)
{
	switch (info->cmdline.operation) {
	case OP_UNDEF:
	case OP_HELP:
		fprintf(stdout, "%s", USAGE);
		break;

	case OP_RUN_FILE:
		fprintf(stdout, "Hello World!!\npath: %s\n", info->cmdline.path);
		break;

	}

	return 0;
}

int main(int argc, char *argv[])
{
	int ret;
	struct app_info info;

	/* アプリケーションの情報を持つ構造体を初期化 */
	app_init_app_info(&info);

	ret = app_parse_cmdline(&info, argc, argv);
	if (ret != 0) {
		fprintf(stderr, "[E %d] Cannot parse.\n", ret);
		return ret;
	}

	ret = init_app(&info);
	if (ret != 0) {
		fprintf(stderr, "[E %d] Cannot init application.\n", ret);
		return ret;
	}

	ret = main_proc(&info);
	if (ret != 0) {
		fprintf(stderr, "[E %d] Cannot execute main_proc.\n", ret);
		return ret;
	}

	ret = deinit_app(&info);
	if (ret != 0) {
		fprintf(stderr, "[W %d] deinit.\n", ret);
	}

	return 0;
}
