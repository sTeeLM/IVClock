#ifndef __IVCLOCK_SM_SET_DATE_H__
#define __IVCLOCK_SM_SET_DATE_H__

#include "task.h"

extern const char * sm_states_names_set_date[];
extern struct sm_trans * sm_trans_set_date[];

enum sm_states_set_date
{
  SM_SET_DATE_INIT, 
  SM_SET_DATE_YEAR,
  SM_SET_DATE_MON,
  SM_SET_DATE_DATE,  
};

void do_set_date_init(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev);

#endif
