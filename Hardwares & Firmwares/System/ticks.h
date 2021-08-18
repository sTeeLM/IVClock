#ifndef __IVCLOCK_TICKS_H__
#define __IVCLOCK_TICKS_H__

#include <stdint.h>

void ticks_init(void);
void ticks_inc_1ms(void);

uint64_t clock_get_now_ms(void);
uint64_t clock_diff_now_ms(uint64_t ms);

#endif
