#ifndef __IVCLOCK_DISPLAY_H__
#define __IVCLOCK_DISPLAY_H__

#include <stdint.h>
#include "clock.h"
#include "timer.h"
#include "alarm.h"
#include "iv18.h"

#define DISPLAY_BLANK  BSP_IV18_BLANK
#define DISPLAY_DEGREE BSP_IV18_DEGREE

void display_init(void);
bool display_is_on(void);
void display_on(void);
void display_off(void);

void display_mon_light_enable(bool enable);
bool display_mon_light_enabled(void);

void display_format_clock(struct clock_struct * clk);
void display_format_timer(struct timer_struct * tmr);
void display_format_alarm0(struct alarm0_struct * alarm0);

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

void display_set_blink_clock_year(bool enable);
void display_set_blink_clock_mon(bool enable);
void display_set_blink_clock_date(bool enable);
void display_set_blink_clock_hour(bool enable);
void display_set_blink_clock_min(bool enable);
void display_set_blink_clock_sec(bool enable);

void display_set_blink_alarm_hour(bool enable);
void display_set_blink_alarm_min(bool enable);
void display_set_blink_alarm_day(bool enable, uint8_t day);
void display_set_blink_alarm_snd(bool enable);

void display_show_string(uint8_t index, const char * str);

enum display_mode
{
  DISPLAY_MODE_NONE = 0,
  DISPLAY_MODE_CLOCK_HHMMSS,
  DISPLAY_MODE_CLOCK_YYMMDD,
  DISPLAY_MODE_CLOCK_WEEK,
  DISPLAY_MODE_TIMER_HHMMSS,
  DISPLAY_MODE_TIMER_MMSSMM,
  DISPLAY_MODE_ALARM_HHMM,
  DISPLAY_MODE_ALARM_DAY, 
  DISPLAY_MODE_ALARM_SND   
};

void display_set_mode(enum display_mode mode);
enum display_mode display_get_mode(void);

#endif