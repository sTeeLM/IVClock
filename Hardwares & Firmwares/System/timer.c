#include "timer.h"
#include "display.h"
#include "debug.h"
#include "task.h"
#include "config.h"
#include "player.h"
#include <string.h>

struct timer_struct tmr[TIMER_SLOT_CNT]; // slot0 当前timer，1/2是两个瞬时值

static enum timer_mode tmr_mode;
static bool refresh_display;
static bool tmr_countdown_stop;
static bool tmr_start;
static uint8_t tmr_current_slot;
static uint8_t tmr_current_history_slot;
static uint8_t tmr_snd;

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
  timer_refresh_display(0);
}

void timer_init(void)
{
  IVDBG("timer_initialize");
  tmr_mode = TIMER_MODE_DEC;
  refresh_display = FALSE;
  tmr_countdown_stop = 1;
  tmr_start = 0;
  tmr_current_slot = 1;
  tmr_current_history_slot = 1;
  tmr_snd = config_read_int("tmr_snd");
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

void timer_refresh_display_enable(bool enable)
{
  refresh_display = enable;
}

void timer_refresh_display(uint8_t slot)
{
  if(refresh_display && display_is_on()) {
    if(slot >= TIMER_SLOT_CNT)
      slot = TIMER_SLOT_CNT - 1;
    display_format_timer(&tmr[slot]);
  } 
}

uint8_t timer_next_history(void)
{
  if(tmr_current_history_slot >= TIMER_SLOT_CNT)
    tmr_current_history_slot = 1;
  return tmr_current_history_slot ++;
}

void timer_rwind_history(void)
{
  tmr_current_history_slot = 1;
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

uint8_t timer_save(void)
{
  if(tmr_current_slot >= TIMER_SLOT_CNT) {
    tmr_current_slot = 1;
  }
  memcpy(&tmr[tmr_current_slot], &tmr[0], sizeof(tmr[0]));
  return tmr_current_slot ++;
}

uint8_t timer_get_slot_cnt(void)
{
  return TIMER_SLOT_CNT;
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

void timer_resume(void)
{
  tmr_start = 1;
}

void timer_clr(void)
{
  memset(tmr, 0, sizeof(tmr));
  tmr_countdown_stop = 1;
  tmr_start = 0;
  tmr_current_slot = 1;
  tmr_current_history_slot = 1;
}

uint8_t timer_get_snd(void)
{
  return tmr_snd;
}

uint8_t timer_inc_snd(void)
{
  tmr_snd ++;
  if(tmr_snd >= player_get_snd_cnt(PLAYER_SND_DIR_EFFETS))
    tmr_snd = 0;
  return tmr_snd;
}

void timer_set_snd(uint8_t snd)
{
  tmr_snd = snd;
  if(tmr_snd >= player_get_snd_cnt(PLAYER_SND_DIR_EFFETS))
    tmr_snd = 0;
}

void timer_play_snd(void)
{
   player_play_snd(PLAYER_SND_DIR_EFFETS, tmr_snd);
}

void timer_stop_play(void)
{
  if(player_is_playing()) {
     player_stop_play();
  }
}

void timer_save_config(void)
{
  config_val_t val;
  val.val8 = tmr_snd;
  config_write("tmr_snd", &val);
}

