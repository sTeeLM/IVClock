#include "sm_clock_display.h"
#include "sm_common.h"
#include "sm_stop_watch.h"
#include "sm_set_time.h"
#include "task.h"
#include "sm.h"
#include "debug.h"
#include "display.h"
#include "clock.h"
#include "thermometer.h"
#include "power.h"
#include "config.h"


void do_clock_display_init(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  display_clr();
  power_reset_timeo();
}

static void do_clock_display_time(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  if(EV_1S == ev) {
    power_test_powersave();
  } else {
    power_reset_timeo();
    clock_refresh_display_enable(FALSE);
    display_clr();
    display_set_mode(DISPLAY_MODE_CLOCK_HHMMSS);
    clock_refresh_display_enable(TRUE);
  }
}

static void do_clock_display_date(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  if(EV_1S == ev) {
    power_test_powersave();
  } else {
    power_reset_timeo();
    clock_refresh_display_enable(FALSE);
    display_clr();
    display_set_mode(DISPLAY_MODE_CLOCK_YYMMDD);
    clock_refresh_display_enable(TRUE);
  }
}

static void do_clock_display_week(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  if(EV_1S == ev) {
    power_test_powersave();
  } else {
    power_reset_timeo();
    clock_refresh_display_enable(FALSE);
    display_clr();
    display_set_mode(DISPLAY_MODE_CLOCK_WEEK);
    clock_refresh_display_enable(TRUE);
  }
}

static void do_clock_display_temp(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{

  if(EV_1S == ev) {
    power_test_powersave();
  } else {
    power_reset_timeo();
    clock_refresh_display_enable(FALSE);
    display_clr();
    thermometer_display();
  }
}

const char * sm_states_names_clock_display[] = {
  "SM_CLOCK_DISPLAY_INIT",
  "SM_CLOCK_DISPLAY_TIME",
  "SM_CLOCK_DISPLAY_DATE",
  "SM_CLOCK_DISPLAY_WEEK",  
  "SM_CLOCK_DISPLAY_TEMP"
};

static struct sm_trans sm_trans_clock_display_init[] = {
  {EV_250MS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_TIME, do_clock_display_time},  
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_clock_display_time[] = { 
  {EV_1S, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_TIME, do_clock_display_time},
  {EV_BUTTON_MOD_PRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_DATE, do_clock_display_date},
  {EV_BUTTON_SET_PRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_TEMP, do_clock_display_temp},
  {EV_BUTTON_MOD_LPRESS, SM_SET_TIME, SM_SET_TIME_INIT, do_set_time_init}, 
  {EV_BUTTON_SET_LPRESS, SM_STOP_WATCH, SM_STOP_WATCH_INIT, do_stop_watch_init},   
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_clock_display_date[] = {   
  {EV_1S, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_DATE, do_clock_display_date},
  {EV_BUTTON_MOD_PRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_WEEK, do_clock_display_week},
  {EV_BUTTON_SET_PRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_TIME, do_clock_display_time},  
  {EV_BUTTON_MOD_LPRESS, SM_SET_TIME, SM_SET_TIME_INIT, do_set_time_init},
  {EV_BUTTON_SET_LPRESS, SM_STOP_WATCH, SM_STOP_WATCH_INIT, do_stop_watch_init},  
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_clock_display_week[] = {  
  {EV_1S, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_WEEK, do_clock_display_week},
  {EV_BUTTON_MOD_PRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_TEMP, do_clock_display_temp},
  {EV_BUTTON_SET_PRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_DATE, do_clock_display_date},  
  {EV_BUTTON_MOD_LPRESS, SM_SET_TIME, SM_SET_TIME_INIT, do_set_time_init},
  {EV_BUTTON_SET_LPRESS, SM_STOP_WATCH, SM_STOP_WATCH_INIT, do_stop_watch_init},  
  {NULL, NULL, NULL, NULL}
};


static struct sm_trans sm_trans_clock_display_temp[] = {
  {EV_1S, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_TEMP, do_clock_display_temp},   
  {EV_BUTTON_MOD_PRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_TIME, do_clock_display_time},
  {EV_BUTTON_SET_PRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_WEEK, do_clock_display_week}, 
  {EV_BUTTON_MOD_LPRESS, SM_SET_TIME, SM_SET_TIME_INIT, do_set_time_init},  
  {EV_BUTTON_SET_LPRESS, SM_STOP_WATCH, SM_STOP_WATCH_INIT, do_stop_watch_init},    
  {NULL, NULL, NULL, NULL}
};

struct sm_trans * sm_trans_clock_display[] = {
  sm_trans_clock_display_init,
  sm_trans_clock_display_time,
  sm_trans_clock_display_date,
  sm_trans_clock_display_week,
  sm_trans_clock_display_temp
};