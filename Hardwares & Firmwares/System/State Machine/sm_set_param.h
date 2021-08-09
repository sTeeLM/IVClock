#ifndef __IVCLOCK_SM_SET_PARAM_H__
#define __IVCLOCK_SM_SET_PARAM_H__

#include "task.h"

extern const char * sm_states_names_set_param[];
extern struct sm_trans * sm_trans_set_param[];

enum sm_states_set_param
{
  SM_SET_PARAM_INIT,
  SM_SET_PARAM_BEEPER,
  SM_SET_PARAM_BAOSHI, 
  SM_SET_PARAM_PS, 
  SM_SET_PARAM_HOUR12,   
};

void do_set_param_init(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev);

#endif
