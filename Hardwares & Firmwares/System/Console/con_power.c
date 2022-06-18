#include "con_power.h"
#include "power.h"
#include "console.h"
#include <stdio.h>
#include <string.h>
#include "cext.h"

int8_t con_power(char arg1, char arg2)
{
  if(arg1 == 0) {
    console_printf("power_33: %s power_50: %s power_490: %s power_iv18: %s\r\n",
		power_33_enabled() ? "on" : "off",
		power_50_enabled() ? "on" : "off",
		power_490_enabled() ? "on" : "off",
		power_iv18_enabled() ? "on" : "off"	    
		);
  } else if(!strcmp(console_buf + arg1, "33")) {
    if(!strcmp(console_buf + arg2, "on")) {
      power_33_enable(TRUE);
    } else  if(!strcmp(console_buf + arg2, "off")) {
      power_33_enable(FALSE);
    } else {
      return -1;
    }
  } else if(!strcmp(console_buf + arg1, "50")) {
    if(!strcmp(console_buf + arg2, "on")) {
      power_50_enable(TRUE);
    } else  if(!strcmp(console_buf + arg2, "off")) {
      power_50_enable(FALSE);
    } else {
      return -1;
    }
  } else if(!strcmp(console_buf + arg1, "490")) {
    if(!strcmp(console_buf + arg2, "on")) {
      power_490_enable(TRUE);
    } else  if(!strcmp(console_buf + arg2, "off")) {
      power_490_enable(FALSE);
    } else {
      return -1;
    }
  } else if(!strcmp(console_buf + arg1, "iv18")) {
    if(!strcmp(console_buf + arg2, "on")) {
      power_iv18_enable(TRUE);
    } else  if(!strcmp(console_buf + arg2, "off")) {
      power_iv18_enable(FALSE);
    } else {
      return -1;
    }
  } else if(!strcmp(console_buf + arg1, "cal84")) {
    power_cal_84();
  } else if(!strcmp(console_buf + arg1, "cal0")) {
    power_cal_0();
  } else if(!strcmp(console_buf + arg1, "bat")) {
    console_printf("battery is %f V\r\n", power_get_bat_voltage());
  } else {
    return -1;
  }
  return 0;
}