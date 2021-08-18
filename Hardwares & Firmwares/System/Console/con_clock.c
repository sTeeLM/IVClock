#include "con_clock.h"
#include "console.h"
#include "clock.h"
#include <stdio.h>

int8_t con_clock(char arg1, char arg2)
{
  if(arg1 == 0) {
    clock_show();
  }
  return 0;
}