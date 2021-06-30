#ifndef __IVCLOCK_CONSOLE_H__
#define __IVCLOCK_CONSOLE_H__

#include <stdint.h>
#include "cext.h"

typedef int8_t (*CONSOLE_CMD_PROC)(char arg1, char arg2);

struct console_cmds
{
  char * cmd;
  char * desc;
  char * usage;
  CONSOLE_CMD_PROC proc;
};


bool console_init(void);
void console_run(void);
void console_dump_cmd(void);
int8_t console_search_cmd_by_name(char * cmd);
extern char console_buf[];
extern struct console_cmds cmds[];
#endif