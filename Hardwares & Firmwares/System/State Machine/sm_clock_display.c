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
#include "power.h"
#include "delay.h"

#define DISPLAY_RETURN_TIME_TIMEO (5) //切换到日期等显示模式之后，过5秒自动切回显示时间

static uint32_t curr_return_time_timeo;

void do_clock_display_init(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  if(ev == EV_BUTTON_SET_LPRESS && from_state == SM_CLOCK_DISPLAY_TIME) {
    task_set(EV_POWEROFF);
  } else {
    display_clr();
    power_reset_timeo();
    sm_common_show_function("---F0---");
  }
}

static void display_test_return_time_timeo(void)
{
  uint8_t diff;
  if(curr_return_time_timeo == 0)
    return;
  diff = clock_diff_now_sec(curr_return_time_timeo);
  IVDBG("display_test_return_timeo diff = %d", diff);
  if(diff > DISPLAY_RETURN_TIME_TIMEO) {
    task_set(EV_VT1);
  }
}

static void display_reset_return_time_timeo(void)
{
  curr_return_time_timeo = clock_get_now_sec();
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
    display_test_return_time_timeo();
  } else {
    display_reset_return_time_timeo();
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
    display_test_return_time_timeo();   
  } else {
    display_reset_return_time_timeo();
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
    display_test_return_time_timeo();  
  } else {
    display_reset_return_time_timeo();
    clock_refresh_display_enable(FALSE);
    display_clr();
    display_format_thermo();
  }
}

static void do_clock_display_bat(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{

  if(EV_1S == ev) {
    display_test_return_time_timeo();  
  } else {
    display_reset_return_time_timeo();
    clock_refresh_display_enable(FALSE);
    display_clr();
    display_format_battery();
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
  "SM_CLOCK_DISPLAY_BAT",  
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
  {EV_BUTTON_SET_PRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_BAT, do_clock_display_bat},
  {EV_ALARM0, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_ALARM0, do_clock_display_alarm0}, 
  {EV_ALARM1, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_ALARM1, do_clock_display_alarm1},   
  {EV_BUTTON_MOD_LPRESS, SM_SET_TIME, SM_SET_TIME_INIT, do_set_time_init},
  {EV_BUTTON_SET_LPRESS, SM_SET_TIME, SM_CLOCK_DISPLAY_INIT, do_clock_display_init}, // power off
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_clock_display_date[] = {   
  {EV_1S, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_DATE, do_clock_display_date},
  {EV_VT1, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_TIME, do_clock_display_time},   
  {EV_BUTTON_MOD_PRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_WEEK, do_clock_display_week},
  {EV_BUTTON_SET_PRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_TIME, do_clock_display_time},  
  {EV_BUTTON_MOD_LPRESS, SM_SET_TIME, SM_SET_TIME_INIT, do_set_time_init}, 
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_clock_display_week[] = {  
  {EV_1S, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_WEEK, do_clock_display_week},
  {EV_VT1, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_TIME, do_clock_display_time},   
  {EV_BUTTON_MOD_PRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_TEMP, do_clock_display_temp},
  {EV_BUTTON_SET_PRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_DATE, do_clock_display_date},  
  {EV_BUTTON_MOD_LPRESS, SM_SET_TIME, SM_SET_TIME_INIT, do_set_time_init},  
  {NULL, NULL, NULL, NULL}
};


static struct sm_trans sm_trans_clock_display_temp[] = {
  {EV_1S, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_TEMP, do_clock_display_temp}, 
  {EV_VT1, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_TIME, do_clock_display_time}, 
  {EV_BUTTON_MOD_PRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_BAT, do_clock_display_bat},
  {EV_BUTTON_SET_PRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_WEEK, do_clock_display_week}, 
  {EV_BUTTON_MOD_LPRESS, SM_SET_TIME, SM_SET_TIME_INIT, do_set_time_init},  
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_clock_display_bat[] = {
  {EV_1S, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_BAT, do_clock_display_bat}, 
  {EV_VT1, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_TIME, do_clock_display_time}, 
  {EV_BUTTON_MOD_PRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_TIME, do_clock_display_time},
  {EV_BUTTON_SET_PRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_TEMP, do_clock_display_temp}, 
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
  sm_trans_clock_display_bat,  
  sm_trans_clock_display_alarm0,
  sm_trans_clock_display_alarm1,  
};