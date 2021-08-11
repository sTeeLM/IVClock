#ifndef __IVCLOCK_SM_SET_ALARM_H__
#define __IVCLOCK_SM_SET_ALARM_H__

#include "task.h"

extern const char * sm_states_names_set_alarm[];
extern struct sm_trans * sm_trans_set_alarm[];

enum sm_states_set_alarm
{
  SM_SET_ALARM_INIT,
  SM_SET_ALARM_SEL,
  SM_SET_ALARM_HOUR, 
  SM_SET_ALARM_MIN,  
  SM_SET_ALARM_DAY,
  SM_SET_ALARM_SND,  
};

void do_set_alarm_init(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev);

#endif
