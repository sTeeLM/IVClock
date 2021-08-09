#ifndef __IVCLOCK_SM_SET_TIME_H__
#define __IVCLOCK_SM_SET_TIME_H__

#include "task.h"

enum sm_states_set_time
{
  SM_SET_TIME_INIT, 
  SM_SET_TIME_HOUR, 
  SM_SET_TIME_MIN,
  SM_SET_TIME_SEC,  
};

void do_set_time_init(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev);

extern const char * sm_states_names_set_time[];
extern struct sm_trans * sm_trans_set_time[];

#endif
