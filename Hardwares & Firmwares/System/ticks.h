#ifndef __IVCLOCK_TICKS_H__
#define __IVCLOCK_TICKS_H__

#include <stdint.h>

void ticks_init(void);
void ticks_inc_1ms(void);

uint64_t ticks_get_now_ms(void);
uint64_t ticks_diff_now_ms(uint64_t ms);
void ticks_enter_console(void);
void ticks_leave_console(void);
void ticks_suspend(void);
void ticks_resume(void);
#endif
