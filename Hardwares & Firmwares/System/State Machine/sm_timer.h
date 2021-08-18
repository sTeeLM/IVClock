#ifndef __IVCLOCK_SM_TIMER_H__
#define __IVCLOCK_SM_TIMER_H__

#include "task.h"

extern const char * sm_states_names_timer[];
extern struct sm_trans * sm_trans_timer[];

enum sm_states_timer
{
  SM_TIMER_INIT, 
  SM_TIMER_SET_HOUR,
  SM_TIMER_SET_MIN,
  SM_TIMER_SET_SEC,  
  SM_TIMER_RUN,
  SM_TIMER_PAUSE,
  SM_TIMER_STOP,  
};

void do_timer_init(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev);

#endif
