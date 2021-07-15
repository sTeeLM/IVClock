#ifndef __IVCLOCK_CLOCK_H__
#define __IVCLOCK_CLOCK_H__

#include <stdint.h>
#include "task.h"
#include "cext.h"

void clock_init (void);
void clock_enter_powersave(void);
void clock_leave_powersave(void);
void clock_enter_shell(void);
void clock_leave_shell(void);
void clock_time_proc(enum task_events ev);

enum clock_sync_type
{
  CLOCK_SYNC_TIME = 0,
  CLOCK_SYNC_DATE = 1
};

struct clock_struct 
{
  uint8_t year;   // 0 - 99 (2000 ~ 2099)
  uint8_t mon;    // 0 - 11
  uint8_t date;   // 0 - 30(29/28/27)
  uint8_t day;    // 0 - 6
  uint8_t hour;   // 0 - 23
  uint8_t ispm;   // 0 if 00:00:00-11:59:59, 1 if 12:00:00-23:59:59
  uint8_t hour12; // 1-12
  uint8_t min;    // 0 - 59
  uint8_t sec;    // 0 - 59
  uint8_t ms39;   // 0 - 255
  uint8_t is12;   // 12 hour format or 24 hour format
};

void clock_show(void);

uint8_t clock_get_hour12(void);
bool clock_is_hour_12format(void);
void clock_set_hour_12format(bool enable);
bool clock_is_pm(void);
uint8_t clock_get_sec(void);
uint8_t clock_get_sec_256(void);
void clock_clr_sec(void);
uint8_t clock_get_min(void);
void clock_inc_min(void);
uint8_t clock_get_hour(void);
void clock_inc_hour(void);
uint8_t clock_get_date(void);
void clock_inc_date(void);
uint8_t clock_get_day(void);
uint8_t clock_get_month(void);
void clock_inc_month(void);
uint8_t clock_get_year(void);
void clock_inc_year(void);
void clock_sync_from_rtc(enum clock_sync_type type);
void clock_sync_to_rtc(enum clock_sync_type type);
void clock_dump(void);
uint8_t clock_get_ms39(void);
void clock_inc_ms39(void);

void clock_enable_interrupt(bool enable);
  
bool clock_is_leap_year(uint8_t year); // year 0-99
uint8_t clock_get_mon_date(uint8_t year, uint8_t mon); // mon 0-11

enum clock_display_mode
{
  CLOCK_DISPLAY_MODE_HHMMSS = 0,
  CLOCK_DISPLAY_MODE_YYMMDD,
  CLOCK_DISPLAY_MODE_WEEK
};

void clock_display(bool enable);
void clock_switch_display_mode(enum clock_display_mode mode);

uint32_t clock_get_now_sec(void);
uint32_t clock_diff_now_sec(uint32_t sec);

#endif