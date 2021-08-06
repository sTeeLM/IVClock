#include "sm_set_alarm.h"
#include "task.h"
#include "sm.h"

static void do_set_alarm_init(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{

}

enum sm_states_set_alarm
{
  SM_SET_ALARM_INIT, 
};

const char * sm_states_names_set_alarm[] = {
  "SM_SET_ALARM_INIT",
};

static struct sm_trans sm_trans_set_alarm_init[] = {
  {EV_BUTTON_MOD_UP, SM_SET_ALARM, SM_SET_ALARM_INIT, do_set_alarm_init},  
  {NULL, NULL, NULL, NULL}
};

struct sm_trans * sm_trans_set_alarm[] = {
  sm_trans_set_alarm_init,
};