#ifndef __IVCLOCK_SM_TIMER_H__
#define __IVCLOCK_SM_TIMER_H__

#include "task.h"

extern const char * sm_states_names_timer[];
extern struct sm_trans * sm_trans_timer[];

enum sm_states_timer
{
  SM_TIMER_INIT, 
};

void do_timer_init(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev);

#endif
