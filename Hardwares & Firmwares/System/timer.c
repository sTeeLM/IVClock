#include "timer.h"
#include "display.h"
#include "debug.h"
#include "task.h"
#include <string.h>

static struct timer_struct tmr[TIMER_SLOT_CNT]; // slot0 当前timer，1/2是两个瞬时值

static enum timer_mode tmr_mode;
static bool refresh_display;
static bool tmr_countdown_stop;
static bool tmr_start;

void timer_inc_ms39(void)
{
  uint8_t ms10;
  
  if(!tmr_start)
    return;
  
  if(tmr_mode == TIMER_MODE_INC) {
    tmr[0].ms39 ++;
    if(tmr[0].ms39 == 0 ) {
      tmr[0].sec = ( ++tmr[0].sec) % 60;
      if(tmr[0].sec == 0) {
        tmr[0].min = (++ tmr[0].min) % 60;
        if(tmr[0].min == 0) {
          tmr[0].hour = (++ tmr[0].hour) % 24;
        }
      }
    }
  } else {
    if(!tmr_countdown_stop) {
      if(tmr[0].ms39 != 0
        || tmr[0].sec != 0
        || tmr[0].min != 0
        || tmr[0].hour != 0) {
        tmr[0].ms39 --;
        if(tmr[0].ms39 == 255 ) {
          tmr[0].sec --;
          if(tmr[0].sec == 255){
            tmr[0].sec = 59;
            tmr[0].min --;
            if(tmr[0].min == 255){ 
              tmr[0].min = 59;
              if(tmr[0].hour > 0) {
                tmr[0].hour --;
              }
            }
          }
        }
      } else {
        tmr_countdown_stop = 1;
        task_set(EV_TIMER);
      }
    }
  }
  if(refresh_display && !display_is_on()) {
    display_format_timer(&tmr[0]);
  } 
}

void timer_init(void)
{
  IVDBG("timer_initialize");
  tmr_mode = TIMER_MODE_DEC;
  refresh_display = FALSE;
  tmr_countdown_stop = 1;
  tmr_start = 0;
}

void timer_enter_powersave(void)
{
  IVDBG("timer_enter_powersave");
  timer_clr(); // 一旦进入节电模式，自动清0，节电模式无法用！
}

void timer_leave_powersave(void)
{
  IVDBG("timer_leave_powersave");
}

void timer_refresh_display(bool enable)
{
  refresh_display = enable;
}
void timer_set_mode(enum timer_mode mode)
{
  tmr_mode = mode;
}

void timer_start(void)
{
  IVDBG("timer_start mode is %u", tmr_mode);
  tmr_countdown_stop = 0;
  tmr_start = 1;
}

void timer_save(uint8_t slot)
{
  if(slot > 0 && slot < TIMER_SLOT_CNT) {
    memcpy(&tmr[slot], &tmr[0], sizeof(tmr[0]));
  }
}

uint8_t timer_get_hour(uint8_t slot)
{
  if(slot >= 0 && slot < TIMER_SLOT_CNT) {
    return tmr[slot].hour;
  }
  return 0;
}
void timer_inc_hour(uint8_t slot)
{
  if(slot >= 0 && slot < TIMER_SLOT_CNT) {
    tmr[slot].hour = (++tmr[slot].hour) % 100;
  }
}

uint8_t timer_get_min(uint8_t slot)
{
  if(slot >= 0 && slot < TIMER_SLOT_CNT) {
    return tmr[slot].min;
  }
  return 0;
}
void timer_inc_min(uint8_t slot)
{
  if(slot >= 0 && slot < TIMER_SLOT_CNT) {
    tmr[slot].min = (++tmr[slot].min) % 60;
  }
}


uint8_t timer_get_sec(uint8_t slot)
{
  if(slot >= 0 && slot < TIMER_SLOT_CNT) {
    return tmr[slot].sec;
  }
  return 0;
}
void timer_inc_sec(uint8_t slot)
{
  if(slot >= 0 && slot < TIMER_SLOT_CNT) {
    tmr[slot].sec = (++ tmr[slot].sec) % 60;
  }
}

uint8_t timer_get_ms10(uint8_t slot)
{
  
  if(slot >= 0 && slot < TIMER_SLOT_CNT) {
    return (uint8_t)(((float)(tmr[slot].ms39) * 3.9) / 10);
  }
  return 0;
}

void timer_stop(void)
{
  tmr_start = 0;
}

void timer_clr(void)
{
  memset(tmr, 0, sizeof(tmr));
  
  refresh_display = FALSE;
  tmr_countdown_stop = 1;
  tmr_start = 0;
}

