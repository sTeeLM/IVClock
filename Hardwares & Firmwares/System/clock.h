#ifndef __IVCLOCK_CLOCK_H__
#define __IVCLOCK_CLOCK_H__

#include <stdint.h>
#include "task.h"
#include "cext.h"

void clock_init (void);
void clock_enter_powersave(void);
void clock_leave_powersave(void);
void clock_enter_console(void);
void clock_leave_console(void);
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
  uint8_t min;    // 0 - 59
  uint8_t sec;    // 0 - 59
  uint8_t ms39;   // 0 - 255
};

extern struct clock_struct clk;

void clock_show(void);

uint8_t clock_get_sec(void);
void clock_set_sec(uint8_t sec);
uint8_t clock_get_sec_256(void);
void clock_clr_sec(void);
uint8_t clock_get_min(void);
void clock_set_min(uint8_t min);
void clock_inc_min(void);
uint8_t clock_get_hour(void);
void clock_set_hour(uint8_t hour);
void clock_inc_hour(void);
uint8_t clock_get_date(void);
void clock_set_date(uint8_t date);
void clock_inc_date(void);
uint8_t clock_get_day(void);
uint8_t clock_get_month(void);
void clock_set_month(uint8_t month);
void clock_inc_month(void);
uint8_t clock_get_year(void);
void clock_set_year(uint8_t year);
void clock_inc_year(void);
void clock_sync_from_rtc(enum clock_sync_type type);
void clock_sync_to_rtc(enum clock_sync_type type);
void clock_dump(void);
uint8_t clock_get_ms39(void);
void clock_inc_ms39(void);

void clock_enable_interrupt(bool enable);
  
bool clock_is_leap_year(uint8_t year); // year 0-99
uint8_t clock_get_mon_date(uint8_t year, uint8_t mon); // mon 0-11


void clock_refresh_display_enable(bool enable);
void clock_refresh_display(void);

uint32_t clock_get_now_sec(void);
uint32_t clock_diff_now_sec(uint32_t sec);

#endif