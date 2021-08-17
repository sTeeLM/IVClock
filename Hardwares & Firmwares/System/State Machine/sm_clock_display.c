#include "sm_clock_display.h"
#include "sm_common.h"
#include "sm_set_time.h"
#include "task.h"
#include "sm.h"
#include "debug.h"
#include "display.h"
#include "clock.h"
#include "thermometer.h"
#include "power.h"
#include "config.h"
#include "player.h"


void do_clock_display_init(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  display_clr();
  power_reset_timeo();
  sm_common_show_function("---F0---");
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
    clock_refresh_display();
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
    clock_refresh_display();
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
    clock_refresh_display();
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

static void do_clock_display_alarm0(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  if(EV_ALARM0 == ev) {
    clock_refresh_display_enable(FALSE);
    display_clr();
    display_set_mode(DISPLAY_MODE_ALARM_HHMM);
    display_format_alarm0(alarm0_get_hit_index());
    alarm0_play_snd(alarm0_get_hit_index());
  } else {
    alarm0_stop_snd();    
    task_set(EV_VT1);
  }
}

static void do_clock_display_alarm1(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  if(EV_ALARM1 == ev) {
    clock_refresh_display_enable(FALSE);
    display_clr();
    display_set_mode(DISPLAY_MODE_ALARM_HHMM);
    display_format_alarm1();
    alarm1_play_snd();
  } else {
    alarm1_stop_snd();
    task_set(EV_VT1);
  }
}

const char * sm_states_names_clock_display[] = {
  "SM_CLOCK_DISPLAY_INIT",
  "SM_CLOCK_DISPLAY_TIME",
  "SM_CLOCK_DISPLAY_DATE",
  "SM_CLOCK_DISPLAY_WEEK",  
  "SM_CLOCK_DISPLAY_TEMP",
  "SM_CLOCK_DISPLAY_ALARM0",
  "SM_CLOCK_DISPLAY_ALARM1"  
};

static struct sm_trans sm_trans_clock_display_init[] = {
  {EV_BUTTON_MOD_UP, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_TIME, do_clock_display_time},  
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_clock_display_time[] = { 
  {EV_1S, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_TIME, do_clock_display_time},
  {EV_BUTTON_MOD_PRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_DATE, do_clock_display_date},
  {EV_BUTTON_SET_PRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_TEMP, do_clock_display_temp},
  {EV_ALARM0, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_ALARM0, do_clock_display_alarm0}, 
  {EV_ALARM1, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_ALARM1, do_clock_display_alarm1},   
  {EV_BUTTON_MOD_LPRESS, SM_SET_TIME, SM_SET_TIME_INIT, do_set_time_init},    
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_clock_display_date[] = {   
  {EV_1S, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_DATE, do_clock_display_date},
  {EV_BUTTON_MOD_PRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_WEEK, do_clock_display_week},
  {EV_BUTTON_SET_PRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_TIME, do_clock_display_time},  
  {EV_BUTTON_MOD_LPRESS, SM_SET_TIME, SM_SET_TIME_INIT, do_set_time_init}, 
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_clock_display_week[] = {  
  {EV_1S, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_WEEK, do_clock_display_week},
  {EV_BUTTON_MOD_PRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_TEMP, do_clock_display_temp},
  {EV_BUTTON_SET_PRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_DATE, do_clock_display_date},  
  {EV_BUTTON_MOD_LPRESS, SM_SET_TIME, SM_SET_TIME_INIT, do_set_time_init},  
  {NULL, NULL, NULL, NULL}
};


static struct sm_trans sm_trans_clock_display_temp[] = {
  {EV_1S, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_TEMP, do_clock_display_temp},   
  {EV_BUTTON_MOD_PRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_TIME, do_clock_display_time},
  {EV_BUTTON_SET_PRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_WEEK, do_clock_display_week}, 
  {EV_BUTTON_MOD_LPRESS, SM_SET_TIME, SM_SET_TIME_INIT, do_set_time_init},  
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_clock_display_alarm0[] = {
  {EV_BUTTON_MOD_PRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_ALARM0, do_clock_display_alarm0},
  {EV_BUTTON_SET_PRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_ALARM0, do_clock_display_alarm0},
  {EV_PLAYER_STOP, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_ALARM0, do_clock_display_alarm0},
  {EV_VT1, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_TIME, do_clock_display_time},  
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_clock_display_alarm1[] = {
  {EV_BUTTON_MOD_PRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_ALARM1, do_clock_display_alarm1},
  {EV_BUTTON_SET_PRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_ALARM1, do_clock_display_alarm1},
  {EV_PLAYER_STOP, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_ALARM1, do_clock_display_alarm1},
  {EV_VT1, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_TIME, do_clock_display_time},  
  {NULL, NULL, NULL, NULL}
};

struct sm_trans * sm_trans_clock_display[] = {
  sm_trans_clock_display_init,
  sm_trans_clock_display_time,
  sm_trans_clock_display_date,
  sm_trans_clock_display_week,
  sm_trans_clock_display_temp,
  sm_trans_clock_display_alarm0,
  sm_trans_clock_display_alarm1,  
};