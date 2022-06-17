#include "con_rtc.h"
#include "console.h"
#include "rtc.h"
#include <stdio.h>

int8_t con_rtc(char arg1, char arg2)
{
  if(arg1 == 0) {
    rtc_show();
  }
  return 0;
}