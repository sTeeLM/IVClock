#include "con_display.h"
#include "console.h"
#include "display.h"
#include "debug.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int8_t con_display(char arg1, char arg2)
{
  if(arg1 == 0) {
    console_printf("display is %s mon light %s\r\n", 
    display_is_on() ? "on" : "off",
    display_mon_light_test_enable() ? "on" : "off");
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
  } else if(!strcmp(console_buf + arg1, "mon")) {
    if(arg2 == 0) {
      console_printf("mon light is %s\r\n", 
      display_mon_light_test_enable() ? "on" : "off");
    } else if(!strcmp(console_buf + arg2, "on")) {
      display_mon_light_set_enable(TRUE);
      display_mon_light_save_config();
    } else if(!strcmp(console_buf + arg2, "off")) {
      display_mon_light_set_enable(FALSE);
      display_mon_light_save_config();
    } else {
      return -1;
    }
  } else if(!strcmp(console_buf + arg1, "lt")) {
    if(arg2 == 0) {
      console_printf("light percent %d\r\n", display_get_light_percent());
    } else {
      display_set_brightness(atoi(console_buf + arg2));
    }
  } else if(!strcmp(console_buf + arg1, "cal0")) {
    display_cal_0();
  } else if(!strcmp(console_buf + arg1, "cal100")) {
    display_cal_100();
  } else if(console_buf[arg1] <= 0x39 && console_buf[arg1] >= 0x30) {
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