#ifndef __IVCLOCK_DISPLAY_H__
#define __IVCLOCK_DISPLAY_H__

#include <stdint.h>
#include "clock.h"
#include "timer.h"

#include "iv18.h"

#define DISPLAY_BLANK  BSP_IV18_BLANK
#define DISPLAY_DEGREE BSP_IV18_DEGREE

void display_init(void);
bool display_is_on(void);
void display_on(void);
void display_off(void);

void display_mon_light_enable(bool enable);
bool display_mon_light_enabled(void);

void display_format_clock(struct clock_struct * clk, enum clock_display_mode display_mode);
void display_format_timer(struct timer_struct * tmr, enum timer_display_mode display_mode);

void display_set_dig(uint8_t index, uint8_t code);
void display_set_dp(uint8_t index);
void display_clr_dp(uint8_t index);
void display_set_blink(uint8_t index);
void display_clr_blink(uint8_t index);
void display_set_brightness(uint8_t percent);
void display_mon_light(void);
uint8_t display_get_light_percent(void);
void display_clr(void);

void display_cal_0(void);
void display_cal_100(void);

void display_enter_powersave(void);
void display_leave_powersave(void);

#endif