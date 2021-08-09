#ifndef __IVCLOCK_SM_ALARM_H__
#define __IVCLOCK_SM_ALARM_H__

#include "task.h"

extern const char * sm_states_names_alarm[];
extern struct sm_trans * sm_trans_alarm[];

enum sm_states_alarm
{
  SM_ALARM_INIT, 
};

void do_alarm_init(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev);

#endif
