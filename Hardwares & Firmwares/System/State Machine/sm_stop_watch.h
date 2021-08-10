#ifndef __IVCLOCK_SM_STOP_WATCH_H__
#define __IVCLOCK_SM_STOP_WATCH_H__

#include "task.h"

enum sm_states_stop_watch
{
  SM_STOP_WATCH_INIT,
  SM_STOP_WATCH_STOP,
  SM_STOP_WATCH_RUN,
  SM_STOP_WATCH_PAUSE,  
};

void do_stop_watch_init(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev);

extern const char * sm_states_names_stop_watch[];
extern struct sm_trans * sm_trans_stop_watch[];

#endif
