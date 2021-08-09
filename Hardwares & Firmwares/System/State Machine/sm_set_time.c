#include "sm_set_time.h"
#include "task.h"
#include "sm.h"
#include "clock.h"
#include "display.h"

#include "sm_set_date.h"

static bool start_inc;

void do_set_time_init(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  display_clr();
  display_set_mode(DISPLAY_MODE_CLOCK_YYMMDD);
  clock_refresh_display(TRUE);
  display_set_blink_clock_hour(TRUE);  
  start_inc  = 0;
}

static void do_set_time_hour(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  switch(ev) {
    case EV_BUTTON_SET_DOWN:
      display_set_blink_clock_hour(FALSE);
      break;
    case EV_BUTTON_SET_LPRESS:
      start_inc = 1;
      clock_inc_hour();
      break;
    case EV_BUTTON_SET_UP:
      display_set_blink_clock_hour(TRUE);
      if(start_inc) {
        clock_sync_to_rtc(CLOCK_SYNC_TIME);
        start_inc = 0;
      }
      break;
    case EV_BUTTON_SET_PRESS:
      clock_inc_hour();
      clock_sync_to_rtc(CLOCK_SYNC_TIME);
      break;
    case EV_BUTTON_MOD_PRESS:
      display_clr();
      display_set_mode(DISPLAY_MODE_CLOCK_HHMMSS);
      clock_refresh_display(TRUE);
      display_set_blink_clock_hour(TRUE); 
      start_inc  = 0;
      break;
    default:;
  }
}

static void do_set_time_min(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  switch(ev) {
    case EV_BUTTON_SET_DOWN:
      display_set_blink_clock_min(FALSE);
      break;
    case EV_BUTTON_SET_LPRESS:
      start_inc = 1;
      clock_inc_min();
      break;
    case EV_BUTTON_SET_UP:
      display_set_blink_clock_min(TRUE);
      if(start_inc) {
        clock_sync_to_rtc(CLOCK_SYNC_TIME);
        start_inc = 0;
      }
      break;
    case EV_BUTTON_SET_PRESS:
      clock_inc_min();
      clock_sync_to_rtc(CLOCK_SYNC_TIME);
      break;
    case EV_BUTTON_MOD_PRESS:
      display_clr();
      display_set_mode(DISPLAY_MODE_CLOCK_HHMMSS);
      clock_refresh_display(TRUE);
      display_set_blink_clock_min(TRUE); 
      start_inc  = 0;
      break;
    default:;
  }
}

static void do_set_time_sec(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  switch(ev) {
    case EV_BUTTON_SET_PRESS:
      clock_clr_sec();
      clock_sync_to_rtc(CLOCK_SYNC_TIME);
      break;
    case EV_BUTTON_MOD_PRESS:
      display_clr();
      display_set_mode(DISPLAY_MODE_CLOCK_HHMMSS);
      clock_refresh_display(TRUE);
      display_set_blink_clock_sec(TRUE); 
      start_inc  = 0;
      break;
    default:;
  }
}

const char * sm_states_names_set_time[] = {
  "SM_SET_TIME_INIT",
  "SM_SET_TIME_HOUR",
  "SM_SET_TIME_MIN",
  "SM_SET_TIME_SEC"
};

static struct sm_trans sm_trans_set_time_init[] = {
  {EV_250MS, SM_SET_TIME, SM_SET_TIME_INIT, do_set_time_hour},  
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_set_time_hour[] = {
  {EV_BUTTON_SET_DOWN, SM_SET_TIME, SM_SET_TIME_HOUR, do_set_time_hour},  
  {EV_BUTTON_SET_PRESS, SM_SET_TIME, SM_SET_TIME_HOUR, do_set_time_hour},
  {EV_BUTTON_SET_LPRESS, SM_SET_TIME, SM_SET_TIME_HOUR, do_set_time_hour}, 
  {EV_BUTTON_SET_UP, SM_SET_TIME, SM_SET_TIME_HOUR, do_set_time_hour},   
  {EV_BUTTON_MOD_PRESS, SM_SET_TIME, SM_SET_TIME_MIN, do_set_time_min},
  {EV_BUTTON_MOD_LPRESS, SM_SET_DATE, SM_SET_DATE_INIT, do_set_date_init},  
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_set_time_min[] = {
  {EV_BUTTON_SET_DOWN, SM_SET_TIME, SM_SET_TIME_HOUR, do_set_time_min},  
  {EV_BUTTON_SET_PRESS, SM_SET_TIME, SM_SET_TIME_HOUR, do_set_time_min},
  {EV_BUTTON_SET_LPRESS, SM_SET_TIME, SM_SET_TIME_HOUR, do_set_time_min}, 
  {EV_BUTTON_SET_UP, SM_SET_TIME, SM_SET_TIME_HOUR, do_set_time_min},   
  {EV_BUTTON_MOD_PRESS, SM_SET_TIME, SM_SET_TIME_MIN, do_set_time_sec}, 
  {EV_BUTTON_MOD_LPRESS, SM_SET_DATE, SM_SET_DATE_INIT, do_set_date_init},   
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_set_time_sec[] = {  
  {EV_BUTTON_SET_PRESS, SM_SET_TIME, SM_SET_TIME_HOUR, do_set_time_sec},  
  {EV_BUTTON_MOD_PRESS, SM_SET_TIME, SM_SET_TIME_MIN, do_set_time_hour}, 
  {EV_BUTTON_MOD_LPRESS, SM_SET_DATE, SM_SET_DATE_INIT, do_set_date_init},   
  {NULL, NULL, NULL, NULL}
};

struct sm_trans * sm_trans_set_time[] = {
  sm_trans_set_time_init,
  sm_trans_set_time_hour,
  sm_trans_set_time_min,
  sm_trans_set_time_sec,
};