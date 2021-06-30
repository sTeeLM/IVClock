#include "sm_clock.h"
#include "task.h"
#include "sm.h"

static void do_clock_init(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev)
{

}

enum sm_states_clock
{
  SM_CLOCK_INIT,
};

const char * sm_states_names_clock[] = {
  "SM_CLOCK_INIT",
};

static struct sm_trans sm_trans_clock_init[] = {
  {EV_250MS, SM_CLOCK, SM_CLOCK_INIT, do_clock_init},
  {NULL, NULL, NULL, NULL}
};

struct sm_trans * sm_trans_clock[] = {
  sm_trans_clock_init,
};