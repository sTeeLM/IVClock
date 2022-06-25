#ifndef __IVCLOCK_SM_CLOCK_DISPLAYH__
#define __IVCLOCK_SM_CLOCK_DISPLAYH__

#include "task.h"

enum sm_states_clock_display
{
  SM_CLOCK_DISPLAY_INIT,
  SM_CLOCK_DISPLAY_TIME,
  SM_CLOCK_DISPLAY_DATE,
  SM_CLOCK_DISPLAY_WEEK,
  SM_CLOCK_DISPLAY_TEMP,
  SM_CLOCK_DISPLAY_BAT,
  SM_CLOCK_DISPLAY_ALARM0,
  SM_CLOCK_DISPLAY_ALARM1
};

void do_clock_display_init(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev);

extern const char * sm_states_names_clock_display[];
extern struct sm_trans * sm_trans_clock_display[];

#endif
