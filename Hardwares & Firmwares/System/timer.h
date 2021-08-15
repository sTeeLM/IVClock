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
extern struct timer_struct tmr[TIMER_SLOT_CNT];

void timer_refresh_display_enable(bool enable);
void timer_refresh_display(uint8_t slot);

uint8_t timer_next_history(void);
void timer_rwind_history(void);

void timer_set_mode(enum timer_mode mode);
void timer_start(void);
uint8_t timer_save(void);
uint8_t timer_get_slot_cnt(void);
uint8_t timer_get_hour(uint8_t slot);
void timer_inc_hour(uint8_t slot);
uint8_t timer_get_min(uint8_t slot);
void timer_inc_min(uint8_t slot);
uint8_t timer_get_sec(uint8_t slot);
void timer_inc_sec(uint8_t slot);
uint8_t timer_get_ms10(uint8_t slot);
void timer_stop(void);
void timer_resume(void);
void timer_clr(void);

uint8_t timer_get_snd(void);
uint8_t timer_inc_snd(void);
void timer_play_snd(void);
void timer_stop_play(void);
void timer_save_config(void);
void timer_set_snd(uint8_t snd);

#endif
