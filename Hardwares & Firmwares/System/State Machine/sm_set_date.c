#include "sm_set_date.h"
#include "sm_common.h"
#include "task.h"
#include "sm.h"

#include "clock.h"
#include "display.h"

#include "sm_set_alarm.h"

static bool start_inc;

void do_set_date_init(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  display_clr();
  clock_refresh_display_enable(FALSE); 
  start_inc  = 0;
  sm_common_show_function("---F2---");
}

static void do_set_date_year(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  switch(ev) {
    case EV_BUTTON_SET_DOWN:
      display_set_blink_clock_year(FALSE);
      break;
    case EV_BUTTON_SET_LPRESS:
      start_inc = 1;
      clock_inc_year();
      clock_refresh_display();
      break;
    case EV_BUTTON_SET_UP:
      display_set_blink_clock_year(TRUE);
      if(start_inc) {
        clock_sync_to_rtc(CLOCK_SYNC_DATE);
        start_inc = 0;
      }
      break;
    case EV_BUTTON_SET_PRESS:
      clock_inc_year();
      clock_refresh_display();
      clock_sync_to_rtc(CLOCK_SYNC_DATE);
      break;
    case EV_BUTTON_MOD_UP:
      if(from_state != SM_SET_DATE_INIT)
        return;
    case EV_BUTTON_MOD_PRESS:
      display_clr();
      display_set_mode(DISPLAY_MODE_CLOCK_YYMMDD);
      clock_refresh_display_enable(TRUE);
      clock_refresh_display();
      display_set_blink_clock_year(TRUE); 
      start_inc  = 0;
      break;
    default:;
  }  
}

static void do_set_date_mon(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  switch(ev) {
    case EV_BUTTON_SET_DOWN:
      display_set_blink_clock_mon(FALSE);
      break;
    case EV_BUTTON_SET_LPRESS:
      start_inc = 1;
      clock_inc_month();
      clock_refresh_display();
      break;
    case EV_BUTTON_SET_UP:
      display_set_blink_clock_mon(TRUE);
      if(start_inc) {
        clock_sync_to_rtc(CLOCK_SYNC_DATE);
        start_inc = 0;
      }
      break;
    case EV_BUTTON_SET_PRESS:
      clock_inc_month();
      clock_refresh_display();
      clock_sync_to_rtc(CLOCK_SYNC_DATE);
      break;
    case EV_BUTTON_MOD_PRESS:
      display_clr();
      display_set_mode(DISPLAY_MODE_CLOCK_YYMMDD);
      clock_refresh_display_enable(TRUE);
      clock_refresh_display();
      display_set_blink_clock_mon(TRUE); 
      start_inc  = 0;
      break;
    default:;
  }  
}

static void do_set_date_date(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  switch(ev) {
    case EV_BUTTON_SET_DOWN:
      display_set_blink_clock_date(FALSE);
      break;
    case EV_BUTTON_SET_LPRESS:
      start_inc = 1;
      clock_inc_date();
      clock_refresh_display();
      break;
    case EV_BUTTON_SET_UP:
      display_set_blink_clock_date(TRUE);
      if(start_inc) {
        clock_sync_to_rtc(CLOCK_SYNC_DATE);
        start_inc = 0;
      }
      break;
    case EV_BUTTON_SET_PRESS:
      clock_inc_date();
      clock_refresh_display();
      clock_sync_to_rtc(CLOCK_SYNC_DATE);
      break;
    case EV_BUTTON_MOD_PRESS:
      display_clr();
      display_set_mode(DISPLAY_MODE_CLOCK_YYMMDD);
      clock_refresh_display_enable(TRUE);
      clock_refresh_display();
      display_set_blink_clock_date(TRUE); 
      start_inc  = 0;
      break;
    default:;
  }  
}

const char * sm_states_names_set_date[] = {
  "SM_SET_DATE_INIT",
  "SM_SET_DATE_YEAR",
  "SM_SET_DATE_MON",
  "SM_SET_DATE_DATE"
};

static struct sm_trans sm_trans_set_date_init[] = {
  {EV_BUTTON_MOD_UP, SM_SET_DATE, SM_SET_DATE_YEAR, do_set_date_year},  
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_set_date_year[] = {
  {EV_BUTTON_SET_DOWN, SM_SET_DATE, SM_SET_DATE_YEAR, do_set_date_year},  
  {EV_BUTTON_SET_PRESS, SM_SET_DATE, SM_SET_DATE_YEAR, do_set_date_year},
  {EV_BUTTON_SET_LPRESS, SM_SET_DATE, SM_SET_DATE_YEAR, do_set_date_year}, 
  {EV_BUTTON_SET_UP, SM_SET_DATE, SM_SET_DATE_YEAR, do_set_date_year},   
  {EV_BUTTON_MOD_PRESS, SM_SET_DATE, SM_SET_DATE_MON, do_set_date_mon},
  {EV_BUTTON_MOD_LPRESS, SM_SET_ALARM, SM_SET_ALARM_INIT, do_set_alarm_init},  
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_set_date_mon[] = {
  {EV_BUTTON_SET_DOWN, SM_SET_DATE, SM_SET_DATE_MON, do_set_date_mon},  
  {EV_BUTTON_SET_PRESS, SM_SET_DATE, SM_SET_DATE_MON, do_set_date_mon},
  {EV_BUTTON_SET_LPRESS, SM_SET_DATE, SM_SET_DATE_MON, do_set_date_mon}, 
  {EV_BUTTON_SET_UP, SM_SET_DATE, SM_SET_DATE_MON, do_set_date_mon},   
  {EV_BUTTON_MOD_PRESS, SM_SET_DATE, SM_SET_DATE_DATE, do_set_date_date}, 
  {EV_BUTTON_MOD_LPRESS, SM_SET_ALARM, SM_SET_ALARM_INIT, do_set_alarm_init},   
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_set_date_date[] = {  
  {EV_BUTTON_SET_DOWN, SM_SET_DATE, SM_SET_DATE_DATE, do_set_date_date},  
  {EV_BUTTON_SET_PRESS, SM_SET_DATE, SM_SET_DATE_DATE, do_set_date_date},
  {EV_BUTTON_SET_LPRESS, SM_SET_DATE, SM_SET_DATE_DATE, do_set_date_date}, 
  {EV_BUTTON_SET_UP, SM_SET_DATE, SM_SET_DATE_DATE, do_set_date_date},   
  {EV_BUTTON_MOD_PRESS, SM_SET_DATE, SM_SET_DATE_YEAR, do_set_date_year}, 
  {EV_BUTTON_MOD_LPRESS, SM_SET_ALARM, SM_SET_ALARM_INIT, do_set_alarm_init},   
  {NULL, NULL, NULL, NULL}
};


struct sm_trans * sm_trans_set_date[] = {
  sm_trans_set_date_init,
  sm_trans_set_date_year,
  sm_trans_set_date_mon,
  sm_trans_set_date_date,
};