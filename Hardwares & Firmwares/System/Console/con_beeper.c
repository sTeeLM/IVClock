#include "con_beeper.h"
#include "console.h"
#include "beeper.h"
#include <stdio.h>
#include <string.h>

int8_t con_beeper(char arg1, char arg2)
{
  int8_t i; 

  if(arg1 == 0) {
    return -1;
  } else if(!strcmp(console_buf + arg1, "b")) {
    beeper_beep();
  } else if(!strcmp(console_buf + arg1, "bb")) {
    beeper_beep_beep();
  } else {
    return -1;
  }
  return 0;
}