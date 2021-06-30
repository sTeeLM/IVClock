#ifndef __IVCLOCK_BUTTON_H__
#define __IVCLOCK_BUTTON_H__

#include <stdint.h>
#include "task.h"

void button_init(void);
void button_scan_proc(enum task_events ev);
void button_mod_proc(enum task_events ev);
void button_set_proc(enum task_events ev);
void button_mod_set_proc(enum task_events ev);

#endif
