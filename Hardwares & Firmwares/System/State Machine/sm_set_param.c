#include "sm_set_param.h"
#include "task.h"
#include "sm.h"
#include "display.h"
#include "beeper.h"
#include "power.h"
#include "config.h"
#include "motion_sensor.h"
#include "player.h"
#include "thermometer.h"
#include "sm_common.h"
#include "delay.h"

#include "sm_timer.h"

void do_set_param_init(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  display_clr();
  alarm0_stop_snd();
  alarm1_stop_snd();
  sm_common_show_function("---F4---");
}

static void do_set_param_beeper(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  if(EV_BUTTON_MOD_PRESS == ev || EV_BUTTON_MOD_UP == ev) {
    display_clr();
    display_format_param_beeper();
  } else {
    beeper_set_enable(!beeper_test_enable());
    beeper_save_config();
    display_format_param_beeper();
  }
}

static void do_set_param_baoshi(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  if(EV_BUTTON_MOD_PRESS == ev && SM_SET_PARAM_BEEPER == from_state) {
    display_clr();
    display_set_mode(DISPLAY_MODE_ALARM_BAOSHI);
    display_format_alarm1();
  } else if(EV_BUTTON_MOD_PRESS == ev) {
    if(!alarm1_test_enable()) {
      task_set(EV_VT1); // goto snd
    } else { 
      task_set(EV_VT2); // goto baosi begin
    }
  } else {
    alarm1_set_enable(!alarm1_test_enable());
    alarm_save_config(ALARM_SYNC_ALARM1, 0);
    display_format_alarm1();
  }
}

static void do_set_param_baoshi_begin(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  if(EV_VT2 == ev) {
    display_clr();
    display_set_mode(DISPLAY_MODE_ALARM_BAOSHI_BEGIN);
    display_format_alarm1();
  } else {
    alarm1_inc_begin();
    alarm_save_config(ALARM_SYNC_ALARM1, 0);
    display_format_alarm1();
  }
}

static void do_set_param_baoshi_end(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  if(EV_BUTTON_MOD_PRESS == ev) {
    display_clr();
    display_set_mode(DISPLAY_MODE_ALARM_BAOSHI_END);
    display_format_alarm1();
  } else {
    alarm1_inc_end();
    alarm_save_config(ALARM_SYNC_ALARM1, 0);
    display_format_alarm1();
  }
}

static void do_set_param_timer_snd(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  if(EV_BUTTON_MOD_PRESS == ev || EV_VT1 == ev) {
    display_clr();
    display_set_mode(DISPLAY_MODE_TIMER_SND);
    display_format_timer(0);
  } else {
    timer_inc_snd();
    timer_save_config();
    timer_stop_play();
    delay_ms(400);
    timer_play_snd();
    display_format_timer(0);
  }
}

static void do_set_param_ps(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  if(EV_BUTTON_MOD_PRESS == ev) {
    display_clr();
    display_format_power();
    timer_stop_play();
  } else {
    power_inc_timeo();
    power_timeo_save_config();
    display_format_power();
  }
}

static void do_set_param_thermo(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  if(EV_BUTTON_MOD_PRESS == ev) {
    display_clr();
    display_format_thermo_unit();
  } else {
    thermometer_set_unit(thermometer_get_unit() == THERMOMETER_UNIT_CEN ? THERMOMETER_UNIT_FAH : THERMOMETER_UNIT_CEN);
    thermometer_save_config();
    display_format_thermo_unit();
  }
}

static void do_set_param_player_vol(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{

  if(EV_BUTTON_MOD_PRESS == ev) {
    display_clr();
    display_format_player_vol();
  } else {
    player_inc_vol();
    player_save_config();
    display_format_player_vol();
  }
}

static void do_set_param_hour12(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  config_val_t val;
  if(EV_BUTTON_MOD_PRESS == ev) {
    display_clr();
    display_format_hour12();
  } else {
    clock_set_hour12(!clock_test_hour12());
    clock_save_config();
    display_format_hour12();
  }
}

static void do_set_param_light_mon(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  if(EV_BUTTON_MOD_PRESS == ev) {
    display_clr();
    display_format_light_mon();
  } else {
    display_mon_light_set_enable(!display_mon_light_test_enable());
    display_mon_light_save_config();
    display_format_light_mon();
  }
}

static void do_set_param_motion_mon(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  if(EV_BUTTON_MOD_PRESS == ev) {
    display_clr();
    display_format_motion_mon();
  } else {
    motion_sensor_set_enable(!motion_sensor_test_enable());
    motion_sensor_save_config();
    display_format_motion_mon();
  }
}

const char * sm_states_names_set_param[] = {
  "SM_SET_PARAM_INIT",
  "SM_SET_PARAM_BEEPER",
  "SM_SET_PARAM_BAOSHI",
  "SM_SET_PARAM_BAOSHI_BEGIN",
  "SM_SET_PARAM_BAOSHI_END",  
  "SM_SET_PARAM_TIMER_SND",
  "SM_SET_PARAM_PS",
  "SM_SET_PARAM_THERMO",
  "SM_SET_PARAM_PLAYER_VOL",
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

static struct sm_trans sm_trans_set_param_baoshi[] = {
  {EV_BUTTON_SET_PRESS, SM_SET_PARAM, SM_SET_PARAM_BAOSHI, do_set_param_baoshi},
  {EV_BUTTON_MOD_PRESS, SM_SET_PARAM, SM_SET_PARAM_BAOSHI, do_set_param_baoshi}, 
  {EV_VT1, SM_SET_PARAM, SM_SET_PARAM_TIMER_SND, do_set_param_timer_snd},  
  {EV_VT2, SM_SET_PARAM, SM_SET_PARAM_BAOSHI_BEGIN, do_set_param_baoshi_begin},      
  {EV_BUTTON_MOD_LPRESS, SM_TIMER, SM_TIMER_INIT, do_timer_init},   
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_set_param_baoshi_begin[] = {
  {EV_BUTTON_SET_PRESS, SM_SET_PARAM, SM_SET_PARAM_BAOSHI_BEGIN, do_set_param_baoshi_begin},
  {EV_BUTTON_MOD_PRESS, SM_SET_PARAM, SM_SET_PARAM_BAOSHI_END, do_set_param_baoshi_end}, 
  {EV_BUTTON_MOD_LPRESS, SM_TIMER, SM_TIMER_INIT, do_timer_init},   
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_set_param_baoshi_end[] = {
  {EV_BUTTON_SET_PRESS, SM_SET_PARAM, SM_SET_PARAM_BAOSHI_END, do_set_param_baoshi_end},
  {EV_BUTTON_MOD_PRESS, SM_SET_PARAM, SM_SET_PARAM_TIMER_SND, do_set_param_timer_snd}, 
  {EV_BUTTON_MOD_LPRESS, SM_TIMER, SM_TIMER_INIT, do_timer_init},   
  {NULL, NULL, NULL, NULL}
};


static struct sm_trans sm_trans_set_param_timer_snd[] = {
  {EV_BUTTON_SET_PRESS, SM_SET_PARAM, SM_SET_PARAM_TIMER_SND, do_set_param_timer_snd},
  {EV_BUTTON_MOD_PRESS, SM_SET_PARAM, SM_SET_PARAM_PS, do_set_param_ps}, 
  {EV_BUTTON_MOD_LPRESS, SM_TIMER, SM_TIMER_INIT, do_timer_init},   
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_set_param_ps[] = {
  {EV_BUTTON_SET_PRESS, SM_SET_PARAM, SM_SET_PARAM_PS, do_set_param_ps},
  {EV_BUTTON_MOD_PRESS, SM_SET_PARAM, SM_SET_PARAM_THERMO, do_set_param_thermo}, 
  {EV_BUTTON_MOD_LPRESS, SM_TIMER, SM_TIMER_INIT, do_timer_init},  
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_set_param_thermo[] = {
  {EV_BUTTON_SET_PRESS, SM_SET_PARAM, SM_SET_PARAM_THERMO, do_set_param_thermo},
  {EV_BUTTON_MOD_PRESS, SM_SET_PARAM, SM_SET_PARAM_PLAYER_VOL, do_set_param_player_vol}, 
  {EV_BUTTON_MOD_LPRESS, SM_TIMER, SM_TIMER_INIT, do_timer_init},  
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_set_param_player_vol[] = {
  {EV_BUTTON_SET_PRESS, SM_SET_PARAM, SM_SET_PARAM_PLAYER_VOL, do_set_param_player_vol},
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
  {EV_BUTTON_SET_PRESS, SM_SET_PARAM, SM_SET_PARAM_MOTION_MON,  do_set_param_motion_mon},
  {EV_BUTTON_MOD_PRESS, SM_SET_PARAM, SM_SET_PARAM_BEEPER, do_set_param_beeper}, 
  {EV_BUTTON_MOD_LPRESS, SM_TIMER, SM_TIMER_INIT, do_timer_init},  
  {NULL, NULL, NULL, NULL}
};

struct sm_trans * sm_trans_set_param[] = {
  sm_trans_set_param_init,
  sm_trans_set_param_beeper,
  sm_trans_set_param_baoshi,
  sm_trans_set_param_baoshi_begin,
  sm_trans_set_param_baoshi_end,
  sm_trans_set_param_timer_snd,
  sm_trans_set_param_ps,
  sm_trans_set_param_thermo,
  sm_trans_set_param_player_vol,
  sm_trans_set_param_hour12,
  sm_trans_set_param_light_mon,
  sm_trans_set_param_motion_mon
};