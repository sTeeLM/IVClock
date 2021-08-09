#include "sm_timer.h"
#include "task.h"
#include "sm.h"

void do_timer_init(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{

}

const char * sm_states_names_timer[] = {
  "SM_SET_TIMER_INIT",
};

static struct sm_trans sm_trans_timer_init[] = {
  {EV_BUTTON_MOD_UP, SM_TIMER, SM_TIMER_INIT, do_timer_init},  
  {NULL, NULL, NULL, NULL}
};

struct sm_trans * sm_trans_timer[] = {
  sm_trans_timer_init,
};