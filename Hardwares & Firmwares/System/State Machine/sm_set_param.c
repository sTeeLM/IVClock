#include "sm_set_param.h"
#include "task.h"
#include "sm.h"
#include "display.h"
#include "beeper.h"
#include "power.h"
#include "config.h"
#include "motion_sensor.h"
#include "sm_common.h"

#include "sm_timer.h"

void do_set_param_init(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  display_clr();
  sm_common_show_function("---F4---");
}

static void do_set_param_beeper(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  if(EV_BUTTON_MOD_PRESS == ev || EV_BUTTON_MOD_UP == ev) {
    display_clr();
    display_format_param_beeper();
  } else {
    beeper_enable(!beeper_enabled());
    display_format_param_beeper();
  }
}

static void do_set_param_baoshi(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  if(EV_BUTTON_MOD_PRESS == ev) {
    display_clr();
    display_set_mode(DISPLAY_MODE_ALARM_BAOSHI);
    display_format_alarm1();
  } else {
    alarm1_set_enable(!alarm1_test_enable());
    alarm_save_config(ALARM_SYNC_ALARM1_ENABLE);
    display_format_alarm1();
  }
}

static void do_set_param_ps(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  if(EV_BUTTON_MOD_PRESS == ev) {
    display_clr();
    display_format_power();
  } else {
    power_inc_timeo();
    display_format_power();
  }
}

static void do_set_param_hour12(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  config_val_t val;
  if(EV_BUTTON_MOD_PRESS == ev) {
    display_clr();
    display_format_hour12();
  } else {
    val.val8 = !config_read_int("time_12");
    config_write("time_12", &val);
    display_format_hour12();
  }
}

static void do_set_param_light_mon(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  if(EV_BUTTON_MOD_PRESS == ev) {
    display_clr();
    display_format_light_mon();
  } else {
    display_mon_light_enable(!display_mon_light_enabled());
    display_format_light_mon();
  }
}

static void do_set_param_motion_mon(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  if(EV_BUTTON_MOD_PRESS == ev) {
    display_clr();
    display_format_motion_mon();
  } else {
    motion_sensor_inc_th();
    display_format_motion_mon();
  }
}

const char * sm_states_names_set_param[] = {
  "SM_SET_PARAM_INIT",
  "SM_SET_PARAM_BEEPER",
  "SM_SET_PARAM_BAOSHI",
  "SM_SET_PARAM_PS",
  "SM_SET_PARAM_HOUR12",
  "SM_SET_PARAM_LIGHT_MON",
  "SM_SET_PARAM_MOTION_MON"
};

static struct sm_trans sm_trans_set_param_init[] = {
  {EV_BUTTON_MOD_UP, SM_SET_PARAM, SM_SET_PARAM_BEEPER, do_set_param_beeper},  
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_set_param_beeper[] = {
  {EV_BUTTON_SET_PRESS, SM_SET_PARAM, SM_SET_PARAM_BEEPER, do_set_param_beeper},
  {EV_BUTTON_MOD_PRESS, SM_SET_PARAM, SM_SET_PARAM_BAOSHI, do_set_param_baoshi}, 
  {EV_BUTTON_MOD_LPRESS, SM_TIMER, SM_TIMER_INIT, do_timer_init},   
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_set_param_ps[] = {
  {EV_BUTTON_SET_PRESS, SM_SET_PARAM, SM_SET_PARAM_PS, do_set_param_ps},
  {EV_BUTTON_MOD_PRESS, SM_SET_PARAM, SM_SET_PARAM_HOUR12, do_set_param_hour12}, 
  {EV_BUTTON_MOD_LPRESS, SM_TIMER, SM_TIMER_INIT, do_timer_init},  
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_set_param_hour12[] = {
  {EV_BUTTON_SET_PRESS, SM_SET_PARAM, SM_SET_PARAM_HOUR12, do_set_param_hour12},
  {EV_BUTTON_MOD_PRESS, SM_SET_PARAM, SM_SET_PARAM_LIGHT_MON, do_set_param_light_mon},
  {EV_BUTTON_MOD_LPRESS, SM_TIMER, SM_TIMER_INIT, do_timer_init},  
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_set_param_light_mon[] = {
  {EV_BUTTON_SET_PRESS, SM_SET_PARAM, SM_SET_PARAM_LIGHT_MON, do_set_param_light_mon},
  {EV_BUTTON_MOD_PRESS, SM_SET_PARAM, SM_SET_PARAM_MOTION_MON, do_set_param_motion_mon}, 
  {EV_BUTTON_MOD_LPRESS, SM_TIMER, SM_TIMER_INIT, do_timer_init},  
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_set_param_motion_mon[] = {
  {EV_BUTTON_SET_PRESS, SM_SET_PARAM, SM_SET_PARAM_MOTION_MON, do_set_param_motion_mon},
  {EV_BUTTON_MOD_PRESS, SM_SET_PARAM, SM_SET_PARAM_BEEPER, do_set_param_beeper}, 
  {EV_BUTTON_MOD_LPRESS, SM_TIMER, SM_TIMER_INIT, do_timer_init},  
  {NULL, NULL, NULL, NULL}
};

struct sm_trans * sm_trans_set_param[] = {
  sm_trans_set_param_init,
};