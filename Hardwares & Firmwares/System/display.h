#ifndef __IVCLOCK_DISPLAY_H__
#define __IVCLOCK_DISPLAY_H__

#include <stdint.h>
#include "clock.h"
#include "timer.h"

void display_init(void);
void display_format_clock(struct clock_struct * clk, enum clock_display_mode display_mode);
void display_format_timer(struct timer_struct * tmr, enum timer_display_mode display_mode);
bool display_is_on(void);
void display_on(void);
void display_off(void);
void display_set_dig(uint8_t index, uint8_t code);
void display_set_dp(uint8_t index);
void display_clr_dp(uint8_t index);
void display_set_blink(uint8_t index);
void display_clr_blink(uint8_t index);

#endif