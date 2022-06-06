#include "console.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "usart.h"
#include "debug.h"
#include "clock.h"
#include "ticks.h"

#include "con_help.h"
#include "con_clock.h"
#include "con_motion_sensor.h"
#include "con_power.h"
#include "con_beeper.h"
#include "con_display.h"
#include "con_player.h"
#include "con_alarm.h"

#define CONSOLE_BUFFER_SIZE 41

void console_init(void)
{

}


char console_buf[CONSOLE_BUFFER_SIZE];

static int8_t con_quit(char arg1, char arg2)
{
  console_printf("quit!\n");
  return 0;
}

struct console_cmds cmds[] = 
{
  {"?",  "show help", "?: list cmd\r\n"
                      "? <cmd>: show usage of cmd",
                      con_help},
  {"clk", "show clock", "clk: show clock\r\n", con_clock},
  {"alm", "show alarm", 
                        "alm: show alarm\r\n",
                        con_alarm},
  {"mon", "motion control",
                        "mon: show motion sensor status\r\n"
                        "mon int: read motion sensor isr\r\n",  
                        con_motion_sensor},
  {"pow", "power control", 
                        "pow: show power status\r\n"
                        "pow 33 on | off: 3.3v on/off\r\n"
                        "pow 50 on | off: 5.0v on/off\r\n" 
                        "pow 490 on | off: 49v on/off\r\n"
                        "pow iv18 on | off: iv18 on/off\r\n" 
                        "pow cal65: calibrate 6.5V\r\n"
                        "pow cal90: calibrate 9.0V\r\n"   
                        "pow bat: show volage of battery\r\n",       
                        con_power}, 
  {"be", "beeper control", 
                        "be: show status\r\n"
                        "be b: be\r\n"
                        "be bb: be ber\n" 
                        "be on: beeper enable\r\n"
                        "be off: beeper disable\r\n",    
                        con_beeper},   
  {"dis", "display control", 
                        "dis: show status\r\n"
                        "dis on: display on\r\n"
                        "dis off: display off\r\n"  
                        "dis sdp n: set dp\r\n"
                        "dis cdp n: clr dp off\r\n" 
                        "dis sbl n: set blink\r\n"
                        "dis cbl n: clr blink off\r\n"
                        "dis lt: show light\r\n"    
                        "dis lt n: set brightness 1 ~ 100\r\n" 
                        "dis cal0: calibrate light 0\r\n"   
                        "dis cal100: calibrate light 100\r\n"  
                        "dis mon on | off: mon light on off\r\n"    
                        "dis n c: set dig\r\n",     
                        con_display}, 
  {"ply", "player control", 
                        "ply: show status\r\n"
                        "ply on: player on\r\n"
                        "ply off: player off\n" 
                        "ply clk: report clock\r\n"
                        "ply tmp: report temperature\r\n"  
                        "ply alm n: player alarm sound\r\n" 
                        "ply stp: stop play\r\n",   
                        con_player},   
  {"!", "quit the console", "!", con_quit},
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
    console_printf("%s: %s\n", cmds[i].cmd, cmds[i].desc);
  }
}

static void _call_cmd(char * buf, char arg1, char arg2)
{
  int8_t i;
  i = console_search_cmd_by_name(buf);
  if(i != -1) {
    if(cmds[i].proc(arg1, arg2) != 0) { // C212
      console_printf("%s:\n%s\n", cmds[i].cmd, cmds[i].usage);
    }
  } else {
    console_printf("unknown cmd '%s'\n", buf);
  }
}

static void _console_gets(char * buffer, uint16_t len)
{
  uint16_t i = 0, c;
  while((c = BSP_USART1_Get_Char()) > 0 && i < len) {
    if(c != '\r' && c != '\n' && c != '\b') {
      BSP_USART1_Transmit((uint8_t *)&c, 1);
    } else if(c == '\b' && i > 0) {
      BSP_USART1_Transmit((uint8_t *)&c, 1);
      BSP_USART1_Transmit((uint8_t *)&"\e[K", 3);
    }
    if(c != '\r' && c != '\n' && c != '\b') {
      buffer[i++] = c;
    } else if(c == '\b'){ // backspace
      if(i > 0)
        buffer[--i] = 0;
    } else if(c != '\r'){
      BSP_USART1_Try_Get_Char();
      break;
    }
  }
}

int16_t console_try_get_key(void)
{
  return BSP_USART1_Try_Get_Char();
}

void console_printf(const char * fmt /*format*/, ...)
{
  va_list arg_ptr;
  
  va_start (arg_ptr, fmt); /* format string */
  vprintf(fmt, arg_ptr);
  va_end (arg_ptr);
}

void console_run(void)
{
  char arg1_pos, arg2_pos, c;
  
  if((c = console_try_get_key()) != 'c' || c == 0) {
    return;
  }
  
  console_printf("++++++++++++++++++++++++++++++++++++++++\n");
  console_printf("+             tini CONSOLE             +\n");
  console_printf("++++++++++++++++++++++++++++++++++++++++\n");
  
  // stop the clock
  clock_enter_console();
  ticks_enter_console();
  do {
    console_printf("console>");
    memset(console_buf, 0, sizeof(console_buf));
    _console_gets(console_buf, sizeof(console_buf)-1);
    console_buf[sizeof(console_buf)-1] = 0;
    console_printf("\r\n");
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
    
    console_printf("\r\n");
    
  } while (strcmp(console_buf, "!") != 0);
  
  clock_leave_console(); 
  ticks_leave_console();
}
