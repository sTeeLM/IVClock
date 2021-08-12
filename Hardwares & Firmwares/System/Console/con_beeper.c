#include "con_beeper.h"
#include "console.h"
#include "beeper.h"
#include <stdio.h>
#include <string.h>

int8_t con_beeper(char arg1, char arg2)
{
  int8_t i; 

  if(arg1 == 0) {
    console_printf("beeper is %s\r\n", beeper_test_enable() ? "on" : "off");
  } else if(!strcmp(console_buf + arg1, "b")) {
    beeper_beep();
  } else if(!strcmp(console_buf + arg1, "bb")) {
    beeper_beep_beep();
  } else if(!strcmp(console_buf + arg1, "on")) {
		beeper_set_enable(TRUE);
    beeper_save_config();
	} else if(!strcmp(console_buf + arg1, "off")) {
		beeper_set_enable(FALSE);
    beeper_save_config();
	} else {
    return -1;
  }
  return 0;
}