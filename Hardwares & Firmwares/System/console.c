#include "console.h"
#include <stdio.h>
#include <string.h>
#include "usart.h"

#include "con_help.h"

#define CONSOLE_BUFFER_SIZE 41

bool console_init(void)
{
  return 0;
}


char console_buf[CONSOLE_BUFFER_SIZE];

static int8_t con_quit(char arg1, char arg2)
{
  printf("quit!\n");
  return 0;
}

struct console_cmds cmds[] = 
{
  {"?",  "show help", "?: list cmd\n"
                      "? <cmd>: show usage of cmd", con_help},
  {"!", "quit the console", "ex", con_quit},
}; 


int8_t console_search_cmd_by_name(char * cmd)
{
  char i;
  for (i = 0 ; i < sizeof(cmds)/sizeof(struct console_cmds) ; i ++) {
    if(strcmp(cmd, cmds[i].cmd) == 0) {
      return i;
    }
  }
  return -1;
}

void console_dump_cmd(void)
{
  char i;
  for (i = 0 ; i < sizeof(cmds)/sizeof(struct console_cmds) ; i ++) {
    printf("%s: %s\n", cmds[i].cmd, cmds[i].desc);
  }
}

static void _call_cmd(char * buf, char arg1, char arg2)
{
  int8_t i;
  i = console_search_cmd_by_name(buf);
  if(i != -1) {
    if(cmds[i].proc(arg1, arg2) != 0) { // C212
      printf("%s:\n%s\n", cmds[i].cmd, cmds[i].usage);
    }
  } else {
    printf("unknown cmd '%s'\n", buf);
  }
}

static void _console_gets(char * buffer, uint16_t len)
{
	
}

void console_run(void)
{
  char arg1_pos, arg2_pos;
  
  printf("++++++++++++++++++++++++++++++++++++++++\n");
  printf("+             tini CONSOLE             +\n");
  printf("++++++++++++++++++++++++++++++++++++++++\n");
  
  // stop the clock
//  clock_enter_console();
//  display_save();
//  display_clear();
//  display_set_code(5, 'S');
//  display_set_code(4, 'H'); 
//  display_set_code(3, 'E');  
//  display_set_code(2, 'L'); 
//  display_set_code(1, 'L');  
  do {
    printf("console>");
    
    _console_gets(console_buf, sizeof(console_buf)-1);
    
    console_buf[sizeof(console_buf)-1] = 0;
    
    if(console_buf[0] == 0)
      continue;
    
    arg1_pos = 0;
    arg2_pos = 0;
    
    while(arg1_pos < sizeof(console_buf) && console_buf[arg1_pos] != ' ') arg1_pos ++;
    
    if(arg1_pos >= sizeof(console_buf)) {
      _call_cmd(console_buf, 0, 0);
      continue;
    }
    
    while(arg1_pos < sizeof(console_buf) && console_buf[arg1_pos] == ' ') {
      console_buf[arg1_pos] = 0;
      arg1_pos ++;
    }
    
    if(arg1_pos >= sizeof(console_buf) || console_buf[arg1_pos] == 0) {
      _call_cmd(console_buf, 0, 0);
      continue;
    }
    
    arg2_pos = arg1_pos;
    
    while(arg2_pos < sizeof(console_buf) && console_buf[arg2_pos] != ' ') arg2_pos ++;
    
    if(arg2_pos >= sizeof(console_buf)) {
      _call_cmd(console_buf, arg1_pos, 0);
      continue;
    }
    
    while(arg2_pos < sizeof(console_buf) && console_buf[arg2_pos] == ' ') {
      console_buf[arg2_pos] = 0;
      arg2_pos ++;
    }
    
    if(arg2_pos >= sizeof(console_buf) || console_buf[arg2_pos] == 0) {
      _call_cmd(console_buf, arg1_pos, 0);
      continue;
    }
    
    _call_cmd(console_buf, arg1_pos, arg2_pos);
    
  } while (strcmp(console_buf, "ex") != 0);
  
//  display_clear();
//  display_restore();
//  clock_leave_shell();  
}
