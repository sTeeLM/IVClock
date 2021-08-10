#include "sm_stop_watch.h"
#include "task.h"
#include "sm.h"
#include "display.h"
#include "sm_common.h"

#include "timer.h"

#include "sm_clock_display.h"

void do_stop_watch_init(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  display_clr();
  timer_refresh_display_enable(FALSE);
  sm_common_show_function("---F5---");
}

static void do_stop_watch_stop(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  if(EV_BUTTON_MOD_UP == ev || EV_BUTTON_SET_LPRESS == ev) {
    timer_refresh_display_enable(TRUE);
    display_set_mode(DISPLAY_MODE_TIMER_HHMMSSMM);
    timer_refresh_display(0);
    timer_set_mode(TIMER_MODE_INC);
    timer_clr();
  }
}

static void do_stop_watch_run(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  if(EV_BUTTON_SET_PRESS == ev) {
    if(SM_STOP_WATCH_STOP == from_state) {
      timer_start();
    } else {
      timer_refresh_display(timer_save());
    }
  } else if(EV_BUTTON_MOD_PRESS == ev) {
    timer_resume();
  }
}

static void do_stop_watch_pause(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  if(EV_BUTTON_MOD_PRESS == ev) {
    timer_stop();
  } else if(EV_BUTTON_SET_PRESS == ev) {
    timer_refresh_display(timer_next_history());
  }
}

const char * sm_states_names_stop_watch[] = {
  "SM_STOP_WATCH_INIT",
  "SM_STOP_WATCH_STOP",
  "SM_STOP_WATCH_RUN",
  "SM_STOP_WATCH_PAUSE"
};

static struct sm_trans sm_trans_stop_watch_init[] = {
  {EV_BUTTON_MOD_UP, SM_STOP_WATCH, SM_STOP_WATCH_STOP, do_stop_watch_stop},  
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_stop_watch_stop[] = {
  {EV_BUTTON_SET_PRESS, SM_STOP_WATCH, SM_STOP_WATCH_RUN, do_stop_watch_run}, 
  {EV_BUTTON_MOD_LPRESS, SM_CLOCK_DISPLAY, SM_CLOCK_DISPLAY_INIT, do_clock_display_init},   
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_stop_watch_run[] = {
  {EV_BUTTON_SET_PRESS, SM_STOP_WATCH, SM_STOP_WATCH_RUN, do_stop_watch_run}, // 暂存
  {EV_BUTTON_MOD_PRESS, SM_STOP_WATCH, SM_STOP_WATCH_PAUSE, do_stop_watch_pause},
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_stop_watch_pause[] = {
  {EV_BUTTON_SET_PRESS, SM_STOP_WATCH, SM_STOP_WATCH_PAUSE, do_stop_watch_pause}, // 显示历史暂存记录 
  {EV_BUTTON_MOD_PRESS, SM_STOP_WATCH, SM_STOP_WATCH_RUN, do_stop_watch_run}, // 继续run
  {EV_BUTTON_SET_LPRESS, SM_STOP_WATCH, SM_STOP_WATCH_STOP, do_stop_watch_stop},  // 清零
  {NULL, NULL, NULL, NULL}
};

struct sm_trans * sm_trans_stop_watch[] = {
  sm_trans_stop_watch_init,
};