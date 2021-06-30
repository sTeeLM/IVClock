#ifndef __IVCLOCK_CLOCK_H__
#define __IVCLOCK_CLOCK_H__

#include <stdint.h>
#include "task.h"
#include "cext.h"

bool clock_init(void);
void clock_time_proc(enum task_events ev);
uint32_t clock_get_now_sec(void);
uint32_t clock_diff_now_sec(uint32_t sec);

#endif