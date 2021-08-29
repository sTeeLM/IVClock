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
  }	else if(!strcmp(console_buf + arg1, "int")) {
    motion_sensor_scan();
  } else {
    return -1;
  }
  return 0;
}