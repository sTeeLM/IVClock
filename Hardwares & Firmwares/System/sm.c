#include "sm.h"
#include "sm_clock_display.h"
#include "debug.h"

static const char * sm_functions_names[] = {
  "SM_CLOCK_DISPLAY",    // 时钟功能
  "SM_SET_TIME",         // 闹钟功能
  "SM_SET_DATE",
  "SM_SET_ALARM",
  "SM_SET_PARAM",
  "SM_TIMER",
  "SM_STOP_WATCH",       // 马表功能
  "SM_ALARM",
};

static const char ** sm_states_names[] = {
  sm_states_names_clock_display,
};

static struct sm_trans ** sm_trans_table[] = {
  sm_trans_clock_display,
};

static uint8_t sm_cur_fuction;
static uint8_t sm_cur_state;

void sm_init(void)
{
  sm_cur_fuction = SM_CLOCK_DISPLAY;
  sm_cur_state   = SM_CLOCK_DISPLAY_INIT;
}

void sm_run(enum task_events ev)
{
  uint8_t i;
  uint8_t new_fun, new_state;
  
  struct sm_trans *p = NULL;

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

