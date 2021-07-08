#include "con_display.h"
#include "console.h"
#include "display.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int8_t con_display(char arg1, char arg2)
{
  if(arg1 == 0) {
    console_printf("display is %s\r\n", display_is_on() ? "on" : "off");
  } else if(!strcmp(console_buf + arg1, "on")) {
    display_on();
  } else if(!strcmp(console_buf + arg1, "off")) {
    display_off();
  } else if(!strcmp(console_buf + arg1, "sdp")) {
    if(arg2 == 0) {
      return -1;
    } else {
      display_set_dp(atoi(console_buf + arg2));
    }
  } else if(!strcmp(console_buf + arg1, "cdp")) {
    if(arg2 == 0) {
      return -1;
    } else {
      display_clr_dp(atoi(console_buf + arg2));
    }
  }else if(!strcmp(console_buf + arg1, "sbl")) {
    if(arg2 == 0) {
      return -1;
    } else {
      display_set_blink(atoi(console_buf + arg2));
    }
  } else if(!strcmp(console_buf + arg1, "cbl")) {
    if(arg2 == 0) {
      return -1;
    } else {
      display_clr_blink(atoi(console_buf + arg2));
    }
  } else if(!strcmp(console_buf + arg1, "lt")) {
    if(arg2 == 0) {
      return -1;
    } else {
      display_set_light(atoi(console_buf + arg2));
    }
  } else if(console_buf[arg1] <= 0x30 && console_buf[arg1] >= 0x39) {
    if(arg2 == 0) {
      return -1;
    } else {
      display_set_dig(atoi(console_buf + arg1), console_buf[arg2]);
    }
  } else {
    return -1;
  }
  return 0;
}