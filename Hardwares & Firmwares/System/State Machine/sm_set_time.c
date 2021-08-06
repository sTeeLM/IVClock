#include "sm_set_time.h"
#include "task.h"
#include "sm.h"

void do_set_time_init(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{

}

const char * sm_states_names_set_time[] = {
  "SM_SET_TIME_INIT",
};

static struct sm_trans sm_trans_set_time_init[] = {
  {EV_BUTTON_MOD_UP, SM_SET_TIME, SM_SET_TIME_INIT, do_set_time_init},  
  {NULL, NULL, NULL, NULL}
};

struct sm_trans * sm_trans_set_time[] = {
  sm_trans_set_time_init,
};