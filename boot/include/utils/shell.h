#ifndef	__SHELL_H
#define __SHELL_H
#include <stdint.h>

#define INPUT_BUF_BLOCK_SIZE	128U

#define BUILTIN_CMD_NUM		8U
#define MAX_ARG_SIZE		128U

#define MAX_FILE_NAME_SIZE	15U


struct builtin_cmd {
	char *name;
	int (*arg_parse)(char *arg_out, const char *arg_in);
	int (*exec)(const char *arg);
};

int main_loop();

#endif
