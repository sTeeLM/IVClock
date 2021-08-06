#include "sm_stop_watch.h"
#include "task.h"
#include "sm.h"

void do_stop_watch_init(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{

}

const char * sm_states_names_stop_watch[] = {
  "SM_STOP_WATCH_INIT",
};

static struct sm_trans sm_trans_stop_watch_init[] = {
  {EV_BUTTON_MOD_UP, SM_STOP_WATCH, SM_STOP_WATCH_INIT, do_stop_watch_init},  
  {NULL, NULL, NULL, NULL}
};

struct sm_trans * sm_trans_stop_watch[] = {
  sm_trans_stop_watch_init,
};