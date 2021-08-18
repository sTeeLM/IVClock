#include "sm_common.h"
#include "display.h"
#include "beeper.h"

void sm_common_show_function(const char * fun)
{
  display_show_string(1,fun);
  beeper_beep_beep();
}
