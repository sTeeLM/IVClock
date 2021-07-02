#include "con_help.h"
#include "console.h"
#include <stdio.h>

int8_t con_help(char arg1, char arg2)
{
  int8_t i; 

  if(arg1 == 0) {
    console_dump_cmd();
  } else {
    i = console_search_cmd_by_name(console_buf + arg1);
    if(i != -1) {
      console_printf("usage:\n%s\n", cmds[i].usage);
    } else {
      console_printf("unknown cmd '%s'\n",console_buf + arg1);
    }
  }
  return 0;
}