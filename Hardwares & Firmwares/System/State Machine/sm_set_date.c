#include "sm_set_date.h"
#include "task.h"
#include "sm.h"

static void do_set_date_init(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{

}

enum sm_states_set_date
{
  SM_SET_DATE_INIT, 
};

const char * sm_states_names_set_date[] = {
  "SM_SET_DATE_INIT",
};

static struct sm_trans sm_trans_set_date_init[] = {
  {EV_BUTTON_MOD_UP, SM_SET_TIME, SM_SET_DATE_INIT, do_set_date_init},  
  {NULL, NULL, NULL, NULL}
};

struct sm_trans * sm_trans_set_date[] = {
  sm_trans_set_date_init,
};