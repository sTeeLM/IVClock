#include "sm.h"
#include "sm_clock_display.h"
#include "sm_set_time.h"
#include "sm_set_date.h"
#include "sm_set_alarm.h"
#include "sm_set_param.h"
#include "sm_timer.h"
#include "sm_stop_watch.h"

#include "clock.h"
#include "timer.h"
#include "delay.h"
#include "power.h"
#include "display.h"

#include "debug.h"

static const char * sm_functions_names[] = {
  "SM_CLOCK_DISPLAY",    // 时钟功能
  "SM_SET_TIME",         // 时间设置
  "SM_SET_DATE",         // 日期设置
  "SM_SET_ALARM",        // 闹钟设置
  "SM_SET_PARAM",        // 参数设置
  "SM_TIMER",            // 计时器功能
  "SM_STOP_WATCH",       // 马表功能
};

static const char ** sm_states_names[] = {
  sm_states_names_clock_display,
  sm_states_names_set_time,
  sm_states_names_set_date,
  sm_states_names_set_alarm,
  sm_states_names_set_param,
  sm_states_names_timer,
  sm_states_names_stop_watch
};

static struct sm_trans ** sm_trans_table[] = {
  sm_trans_clock_display,
  sm_trans_set_time,
  sm_trans_set_date,
  sm_trans_set_alarm,
  sm_trans_set_param,
  sm_trans_timer,
  sm_trans_stop_watch
};

static uint8_t sm_cur_fuction;
static uint8_t sm_cur_state;

void sm_init(void)
{
  sm_cur_fuction = SM_CLOCK_DISPLAY;
  sm_cur_state   = SM_CLOCK_DISPLAY_INIT;
  task_set(EV_BUTTON_MOD_UP);
}

void sm_run(enum task_events ev)
{
  uint8_t i;
  uint8_t new_fun, new_state;
  
  struct sm_trans *p = NULL;
  
  if(ev == EV_POWEROFF) {
    clock_refresh_display_enable(FALSE);
    timer_refresh_display_enable(FALSE); 
    display_clr();
    display_format_poweroff();
    delay_ms(5000);
    display_clr();
    power_490_enable(FALSE);
    power_50_enable(FALSE);
    power_33_enable(FALSE);
  } else {
    p = sm_trans_table[sm_cur_fuction][sm_cur_state];
    while(p != NULL && p->sm_proc) {
      if(p->event == ev) {
        IVDBG("[%s] [%s][%s] -> [%s][%s]",
          task_names[ev],
          sm_functions_names[sm_cur_fuction],
          sm_states_names[sm_cur_fuction][sm_cur_state],
          sm_functions_names[p->to_function],
          sm_states_names[p->to_function][p->to_state]     
        );
        p->sm_proc(sm_cur_fuction, sm_cur_state, p->to_function, p->to_state, ev);
        sm_cur_fuction = p->to_function;
        sm_cur_state   = p->to_state;
        break;
      }
      p ++;
    }
  }
}

