#include "con_clock.h"
#include "console.h"
#include "motion_sensor.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int8_t con_motion_sensor(char arg1, char arg2)
{
  uint8_t val; 
  
  if(arg1 == 0) {
    motion_sensor_show();
  } else if(!strcmp(console_buf + arg1, "on")) {
    motion_sensor_on();
  } else if(!strcmp(console_buf + arg1, "off")) {
    motion_sensor_off();
  }	else if(!strcmp(console_buf + arg1, "int")) {
    motion_sensor_isr();
  } else if(!strcmp(console_buf + arg1, "th")) {
    if(arg2 == 0) {
      return -1;
    } else {
      val = atoi(console_buf + arg2);
      motion_sensor_set_th(val);
    }
  } else {
    return -1;
  }
  return 0;
}