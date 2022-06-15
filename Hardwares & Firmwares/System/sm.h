#ifndef __IVCLOCK_SM_H__
#define __IVCLOCK_SM_H__

#include <stdint.h>
#include <stddef.h>
#include "task.h"

enum sm_functions
{
  SM_CLOCK_DISPLAY  = 0,    // 时钟功能
  SM_SET_TIME,              // 闹钟功能
  SM_SET_DATE,
  SM_SET_ALARM,
  SM_SET_PARAM,
  SM_TIMER,
  SM_STOP_WATCH,            // 马表功能
  SM_FUNCTION_CNT
};

typedef void (*SM_PROC)(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev);

struct sm_trans
{
  enum task_events event;
  uint8_t to_function;
  uint8_t to_state;
  SM_PROC sm_proc;
};

void sm_init(void);

void sm_run(enum task_events ev);

#endif
