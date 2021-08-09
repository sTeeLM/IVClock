#include "sm_alarm.h"
#include "task.h"
#include "sm.h"

void do_alarm_init(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{

}

const char * sm_states_names_alarm[] = {
  "SM_ALARM_INIT",
};

static struct sm_trans sm_trans_alarm_init[] = {
  {EV_BUTTON_MOD_UP, SM_ALARM, SM_ALARM_INIT, do_alarm_init},  
  {NULL, NULL, NULL, NULL}
};

struct sm_trans * sm_trans_alarm[] = {
  sm_trans_alarm_init,
};