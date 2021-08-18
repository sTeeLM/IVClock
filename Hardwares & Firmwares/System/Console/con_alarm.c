#include "con_alarm.h"
#include "console.h"
#include "alarm.h"
#include <stdio.h>

int8_t con_alarm(char arg1, char arg2)
{
  if(arg1 == 0) {
    alarm_show();
  }
  return 0;
}