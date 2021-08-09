#ifndef __IVCLOCK_TIMER_H__
#define __IVCLOCK_TIMER_H__

#include <stdint.h>
#include "cext.h"

void timer_init(void);
void timer_enter_powersave(void);
void timer_leave_powersave(void);
void timer_inc_ms39(void);
enum timer_mode {
  TIMER_MODE_INC = 0,
  TIMER_MODE_DEC = 1,  
};

struct timer_struct {
  uint8_t hour;
  uint8_t min;
  uint8_t sec;
  uint8_t ms39;
};

#define TIMER_SLOT_CNT 5

void timer_refresh_display(bool enable);

void timer_set_mode(enum timer_mode mode);
void timer_start(void);
void timer_save(uint8_t slot);
uint8_t timer_get_hour(uint8_t slot);
void timer_inc_hour(uint8_t slot);
uint8_t timer_get_min(uint8_t slot);
void timer_inc_min(uint8_t slot);
uint8_t timer_get_sec(uint8_t slot);
void timer_inc_sec(uint8_t slot);
uint8_t timer_get_ms10(uint8_t slot);
void timer_stop(void);
void timer_clr(void);

#endif