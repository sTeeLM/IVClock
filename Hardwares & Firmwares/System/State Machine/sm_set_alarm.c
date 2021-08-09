#include "sm_set_alarm.h"
#include "task.h"
#include "sm.h"

#include "alarm.h"
#include "display.h"

#include "sm_timer.h"

static bool start_inc;
static uint8_t day_index;
  
void do_set_alarm_init(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  display_clr();
  display_set_mode(DISPLAY_MODE_ALARM_HHMM);
  clock_refresh_display(FALSE);
  display_set_blink_alarm_hour(TRUE); 
  start_inc  = 0;
  day_index = 0;
  display_format_alarm0(&alarm0);
}

static void do_set_alarm_hour(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  switch(ev) {
    case EV_BUTTON_SET_DOWN:
      display_set_blink_alarm_hour(FALSE);
      break;
    case EV_BUTTON_SET_LPRESS:
      start_inc = 1;
      alarm0_inc_hour();
      display_format_alarm0(&alarm0);
      break;
    case EV_BUTTON_SET_UP:
      display_set_blink_alarm_hour(TRUE);
      if(start_inc) {
        alarm0_sync_to_rtc();
        alarm_save_config(ALARM_SYNC_ALARM0_HOUR);
        start_inc = 0;
      }
      break;
    case EV_BUTTON_SET_PRESS:
      alarm0_inc_hour();
      display_format_alarm0(&alarm0);
      alarm0_sync_to_rtc();
      alarm_save_config(ALARM_SYNC_ALARM0_HOUR);
      break;
    case EV_BUTTON_MOD_PRESS:
      display_clr();
      display_set_mode(DISPLAY_MODE_ALARM_HHMM);
      display_set_blink_alarm_hour(TRUE); 
      display_format_alarm0(&alarm0);
      start_inc  = 0;
      day_index = 0;
      break;
    default:;
  } 
}

static void do_set_alarm_min(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  switch(ev) {
    case EV_BUTTON_SET_DOWN:
      display_set_blink_alarm_min(FALSE);
      break;
    case EV_BUTTON_SET_LPRESS:
      start_inc = 1;
      alarm0_inc_min();
      display_format_alarm0(&alarm0);
      break;
    case EV_BUTTON_SET_UP:
      display_set_blink_alarm_min(TRUE);
      if(start_inc) {
        alarm0_sync_to_rtc();
        alarm_save_config(ALARM_SYNC_ALARM0_MIN);
        start_inc = 0;
      }
      break;
    case EV_BUTTON_SET_PRESS:
      alarm0_inc_min();
      display_format_alarm0(&alarm0);
      alarm0_sync_to_rtc();
      alarm_save_config(ALARM_SYNC_ALARM0_MIN);
      break;
    case EV_BUTTON_MOD_PRESS:
      display_clr();
      display_set_mode(DISPLAY_MODE_ALARM_HHMM);
      display_set_blink_alarm_min(TRUE); 
      display_format_alarm0(&alarm0);
      start_inc  = 0;
      day_index = 0;
      break;
    default:;
  } 
}

static void do_set_alarm_day(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  switch(ev) {
    case EV_BUTTON_SET_PRESS:
      alarm0_set_enable(day_index, !alarm0_test_enable(day_index));
      alarm_save_config(ALARM_SYNC_ALARM0_DAY_MASK);
      display_format_alarm0(&alarm0);
      break;
    case EV_BUTTON_MOD_PRESS: 
      if(day_index == 7) {
        task_set(EV_VT1);
        day_index = 0;
      } else if(day_index == 0){
        display_clr();
        display_set_mode(DISPLAY_MODE_ALARM_DAY);
        day_index ++;
        display_set_blink_alarm_day(TRUE, day_index);
        display_format_alarm0(&alarm0);
      } else {
        display_set_blink_alarm_day(FALSE, day_index);
        day_index ++;
        display_set_blink_alarm_day(TRUE, day_index);
        display_format_alarm0(&alarm0);
      }
      break;
      default:;
  }
}

static void do_set_alarm_snd(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  switch(ev) {
    case EV_BUTTON_SET_DOWN:
      display_set_blink_alarm_snd(FALSE);
      break;
    case EV_BUTTON_SET_LPRESS:
      start_inc = 1;
      alarm0_inc_snd();
      display_format_alarm0(&alarm0);
      break;
    case EV_BUTTON_SET_UP:
      display_set_blink_alarm_snd(TRUE);
      if(start_inc) {
        alarm_save_config(ALARM_SYNC_ALARM0_SND);
        start_inc = 0;
      }
      break;
    case EV_BUTTON_SET_PRESS:
      alarm0_inc_snd();
      display_format_alarm0(&alarm0);
      alarm_save_config(ALARM_SYNC_ALARM0_SND);
      break;
    case EV_BUTTON_MOD_PRESS:
      display_clr();
      display_set_mode(DISPLAY_MODE_ALARM_SND);
      display_set_blink_alarm_snd(TRUE); 
      display_format_alarm0(&alarm0);
      start_inc  = 0;
      break;
    default:;
  } 
}

const char * sm_states_names_set_alarm[] = {
  "SM_SET_ALARM_INIT",
  "SM_SET_ALARM_HOUR",
  "SM_SET_ALARM_MIN", 
  "SM_SET_ALARM_DAY", 
  "SM_SET_ALARM_SND"
};

static struct sm_trans sm_trans_set_alarm_init[] = {
  {EV_250MS, SM_SET_ALARM, SM_SET_ALARM_HOUR, do_set_alarm_hour},  
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_set_alarm_hour[] = {
  {EV_BUTTON_SET_DOWN, SM_SET_ALARM, SM_SET_ALARM_HOUR, do_set_alarm_hour},  
  {EV_BUTTON_SET_PRESS, SM_SET_ALARM, SM_SET_ALARM_HOUR, do_set_alarm_hour},
  {EV_BUTTON_SET_LPRESS, SM_SET_ALARM, SM_SET_ALARM_HOUR, do_set_alarm_hour}, 
  {EV_BUTTON_SET_UP, SM_SET_ALARM, SM_SET_ALARM_HOUR, do_set_alarm_hour},   
  {EV_BUTTON_MOD_PRESS, SM_SET_ALARM, SM_SET_ALARM_MIN, do_set_alarm_min},
  {EV_BUTTON_MOD_LPRESS, SM_TIMER, SM_TIMER_INIT, do_timer_init},  
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_set_alarm_min[] = {
  {EV_BUTTON_SET_DOWN, SM_SET_ALARM, SM_SET_ALARM_MIN, do_set_alarm_min},  
  {EV_BUTTON_SET_PRESS, SM_SET_ALARM, SM_SET_ALARM_MIN, do_set_alarm_min},
  {EV_BUTTON_SET_LPRESS, SM_SET_ALARM, SM_SET_ALARM_MIN, do_set_alarm_min}, 
  {EV_BUTTON_SET_UP, SM_SET_ALARM, SM_SET_ALARM_MIN, do_set_alarm_min},   
  {EV_BUTTON_MOD_PRESS, SM_SET_ALARM, SM_SET_ALARM_DAY, do_set_alarm_day},
  {EV_BUTTON_MOD_LPRESS, SM_TIMER, SM_TIMER_INIT, do_timer_init},  
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_set_alarm_day[] = { 
  {EV_BUTTON_SET_PRESS, SM_SET_ALARM, SM_SET_ALARM_DAY, do_set_alarm_day},  
  {EV_BUTTON_MOD_PRESS, SM_SET_ALARM, SM_SET_ALARM_DAY, do_set_alarm_day},
  {EV_VT1, SM_SET_ALARM, SM_SET_ALARM_SND, do_set_alarm_snd},  
  {EV_BUTTON_MOD_LPRESS, SM_TIMER, SM_TIMER_INIT, do_timer_init},  
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_set_alarm_snd[] = {
  {EV_BUTTON_SET_DOWN, SM_SET_ALARM, SM_SET_ALARM_SND, do_set_alarm_snd},  
  {EV_BUTTON_SET_PRESS, SM_SET_ALARM, SM_SET_ALARM_SND, do_set_alarm_snd},
  {EV_BUTTON_SET_LPRESS, SM_SET_ALARM, SM_SET_ALARM_SND, do_set_alarm_snd}, 
  {EV_BUTTON_SET_UP, SM_SET_ALARM, SM_SET_ALARM_SND, do_set_alarm_snd},   
  {EV_BUTTON_MOD_PRESS, SM_SET_ALARM, SM_SET_ALARM_HOUR, do_set_alarm_hour},
  {EV_BUTTON_MOD_LPRESS, SM_TIMER, SM_TIMER_INIT, do_timer_init},  
  {NULL, NULL, NULL, NULL}
};

struct sm_trans * sm_trans_set_alarm[] = {
  sm_trans_set_alarm_init,
  sm_trans_set_alarm_hour,
  sm_trans_set_alarm_min,
  sm_trans_set_alarm_day,
  sm_trans_set_alarm_snd,
};


