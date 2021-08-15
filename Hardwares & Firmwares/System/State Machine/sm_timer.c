#include "sm_timer.h"
#include "task.h"
#include "sm.h"
#include "display.h"
#include "sm_common.h"
#include "debug.h"
#include "player.h"
#include "config.h"

#include "timer.h"

#include "sm_stop_watch.h"

void do_timer_init(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  display_clr();
  timer_refresh_display_enable(FALSE);  
  sm_common_show_function("---F5---");
}

static void do_timer_set_hour(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  switch(ev) {
    case EV_BUTTON_SET_DOWN:
      display_set_blink_timer_hour(FALSE);
      break;
    case EV_BUTTON_SET_PRESS:
      timer_inc_hour(0);
      timer_refresh_display(0);
      break;
    case EV_BUTTON_SET_UP:
      display_set_blink_timer_hour(TRUE);
      break;
    case EV_BUTTON_SET_LPRESS:
      timer_inc_hour(0);
      timer_refresh_display(0);
      break;
    case EV_BUTTON_MOD_UP:
      if(from_state != SM_TIMER_INIT)
        return;
    case EV_BUTTON_MOD_PRESS:
    case EV_VT2:
      display_clr();
      timer_clr();
      timer_refresh_display_enable(TRUE);
      display_set_blink_timer_hour(TRUE);
      display_set_mode(DISPLAY_MODE_TIMER_HHMMSS);
      timer_refresh_display(0);
      timer_set_mode(TIMER_MODE_DEC);
      timer_stop_play();
    default:;
  }
}

static void do_timer_set_min(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  switch(ev) {
    case EV_BUTTON_SET_DOWN:
      display_set_blink_timer_min(FALSE);
      break;
    case EV_BUTTON_SET_LPRESS:
      timer_inc_min(0);
      timer_refresh_display(0);
      break;
    case EV_BUTTON_SET_UP:
      display_set_blink_timer_min(TRUE);
      break;
    case EV_BUTTON_SET_PRESS:
      timer_inc_min(0);
      timer_refresh_display(0);
      break;
    case EV_BUTTON_MOD_PRESS:
      display_clr();
      display_set_blink_timer_min(TRUE); 
      timer_refresh_display(0);
    default:;
  }
}

static void do_timer_set_sec(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  switch(ev) {
    case EV_BUTTON_SET_DOWN:
      display_set_blink_timer_sec(FALSE);
      break;
    case EV_BUTTON_SET_LPRESS:
      timer_inc_sec(0);
      timer_refresh_display(0);
      break;
    case EV_BUTTON_SET_UP:
      display_set_blink_timer_sec(TRUE);
      break;
    case EV_BUTTON_SET_PRESS:
      timer_inc_sec(0);
      timer_refresh_display(0);
      break;
    case EV_BUTTON_MOD_PRESS:
      if(SM_TIMER_SET_MIN == from_state) {
        display_clr();
        display_set_blink_timer_sec(TRUE); 
        timer_refresh_display(0);
      } else {
        if(timer_get_hour(0) != 0 || timer_get_min(0) != 0 || timer_get_sec(0) != 0) {
          task_set(EV_VT1);
        } else {
          task_set(EV_VT2);
        }
      }
    default:;
  } 
}

static void do_timer_run(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  if(EV_VT1 == ev) {
    display_clr();
    display_set_mode(DISPLAY_MODE_TIMER_HHMMSSMM);
    timer_start();
  } else if(EV_BUTTON_SET_PRESS == ev) {
    timer_resume();
  }
}

static void do_timer_pause(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  timer_stop();
}

static void do_timer_stop(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{
  if(ev == EV_TIMER || ev == EV_PLAYER_STOP) {
    timer_play_snd();
    if(ev == EV_TIMER)
      display_set_blink_all(TRUE);   
  }
  timer_stop();
}

const char * sm_states_names_timer[] = {
  "SM_TIMER_INIT",
  "SM_TIMER_SET_HOUR",
  "SM_TIMER_SET_MIN",
  "SM_TIMER_SET_SEC",  
  "SM_TIMER_RUN",
  "SM_TIMER_PAUSE",
  "SM_TIMER_STOP",
};

static struct sm_trans sm_trans_timer_init[] = {
  {EV_BUTTON_MOD_UP, SM_TIMER, SM_TIMER_SET_HOUR, do_timer_set_hour},  
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_timer_set_hour[] = {
  {EV_BUTTON_SET_DOWN, SM_TIMER, SM_TIMER_SET_HOUR, do_timer_set_hour},  
  {EV_BUTTON_SET_PRESS, SM_TIMER, SM_TIMER_SET_HOUR, do_timer_set_hour},
  {EV_BUTTON_SET_LPRESS, SM_TIMER, SM_TIMER_SET_HOUR, do_timer_set_hour},  
  {EV_BUTTON_SET_UP, SM_TIMER, SM_TIMER_SET_HOUR, do_timer_set_hour},
  {EV_BUTTON_MOD_PRESS, SM_TIMER, SM_TIMER_SET_MIN, do_timer_set_min},
  {EV_BUTTON_MOD_LPRESS, SM_STOP_WATCH, SM_STOP_WATCH_INIT, do_stop_watch_init}, 
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_timer_set_min[] = {
  {EV_BUTTON_SET_DOWN, SM_TIMER, SM_TIMER_SET_MIN, do_timer_set_min},  
  {EV_BUTTON_SET_PRESS, SM_TIMER, SM_TIMER_SET_MIN, do_timer_set_min},
  {EV_BUTTON_SET_LPRESS, SM_TIMER, SM_TIMER_SET_MIN, do_timer_set_min},  
  {EV_BUTTON_SET_UP, SM_TIMER, SM_TIMER_SET_MIN, do_timer_set_min},
  {EV_BUTTON_MOD_PRESS, SM_TIMER, SM_TIMER_SET_SEC, do_timer_set_sec},
  {EV_BUTTON_MOD_LPRESS, SM_STOP_WATCH, SM_STOP_WATCH_INIT, do_stop_watch_init}, 
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_timer_set_sec[] = {
  {EV_BUTTON_SET_DOWN, SM_TIMER, SM_TIMER_SET_SEC, do_timer_set_sec},  
  {EV_BUTTON_SET_PRESS, SM_TIMER, SM_TIMER_SET_SEC, do_timer_set_sec},
  {EV_BUTTON_SET_LPRESS, SM_TIMER, SM_TIMER_SET_SEC, do_timer_set_sec},  
  {EV_BUTTON_SET_UP, SM_TIMER, SM_TIMER_SET_SEC, do_timer_set_sec},
  {EV_BUTTON_MOD_PRESS, SM_TIMER, SM_TIMER_SET_SEC, do_timer_set_sec},
  {EV_VT1, SM_TIMER, SM_TIMER_RUN, do_timer_run},
  {EV_VT2, SM_TIMER, SM_TIMER_SET_HOUR, do_timer_set_hour},
  {EV_BUTTON_MOD_LPRESS, SM_STOP_WATCH, SM_STOP_WATCH_INIT, do_stop_watch_init}, 
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_timer_run[] = {
  {EV_BUTTON_SET_PRESS, SM_TIMER, SM_TIMER_PAUSE, do_timer_pause},
  {EV_TIMER, SM_TIMER, SM_TIMER_STOP, do_timer_stop},  
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_timer_pause[] = {
  {EV_BUTTON_SET_PRESS, SM_TIMER, SM_TIMER_RUN, do_timer_run},
  {EV_BUTTON_MOD_PRESS, SM_TIMER, SM_TIMER_STOP, do_timer_stop},  
  {NULL, NULL, NULL, NULL}
};

static struct sm_trans sm_trans_timer_stop[] = {
  {EV_BUTTON_MOD_PRESS, SM_TIMER, SM_TIMER_SET_HOUR, do_timer_set_hour},
  {EV_PLAYER_STOP, SM_TIMER, SM_TIMER_STOP, do_timer_stop},
  {NULL, NULL, NULL, NULL}
};

struct sm_trans * sm_trans_timer[] = {
  sm_trans_timer_init,
  sm_trans_timer_set_hour,
  sm_trans_timer_set_min,
  sm_trans_timer_set_sec,  
  sm_trans_timer_run,
  sm_trans_timer_pause,
  sm_trans_timer_stop,
};