#ifndef APP_H_
#define APP_H_

enum OPERATIONS_NUM {
	OP_UNDEF = 0,
	OP_HELP,
	OP_RUN_FILE
};

/* コマンドラインからの情報を格納する構造体 */
struct app_cmdline {
	const char *path;
	int operation;
};

/* アプリケーションの情報を格納する構造体 */
struct app_info {
	struct app_cmdline cmdline;
};

#endif /* APP_H_*/
