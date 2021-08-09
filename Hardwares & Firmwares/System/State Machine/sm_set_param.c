#include "sm_set_param.h"
#include "task.h"
#include "sm.h"

void do_set_param_init(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{

}

const char * sm_states_names_set_param[] = {
  "SM_SET_PARAM_INIT",
};

static struct sm_trans sm_trans_set_param_init[] = {
  {EV_BUTTON_MOD_UP, SM_SET_PARAM, SM_SET_PARAM_INIT, do_set_param_init},  
  {NULL, NULL, NULL, NULL}
};

struct sm_trans * sm_trans_set_param[] = {
  sm_trans_set_param_init,
};