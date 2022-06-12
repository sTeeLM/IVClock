#include <stdint.h>
#include <stdlib.h>
#include "ticks.h"
#include "bsp.h"
#include "task.h"


static uint64_t _ticks;

void ticks_init(void)
{
  _ticks = rand();
}

uint64_t ticks_get_now_ms(void)
{
  return _ticks;
}

uint64_t ticks_diff_now_ms(uint64_t ms)
{
  return (uint64_t)(ms - _ticks);
}

void ticks_inc_1ms(void)
{
  _ticks ++;
  
  if((_ticks % 23) == 0)
    task_set(EV_BUTTON_SCAN);
  
  if((_ticks % 250) == 0)
    task_set(EV_250MS);
  
  if((_ticks % 1000) == 0)
    task_set(EV_1S);
}

void ticks_enter_console(void)
{
  ticks_suspend();
}

void ticks_leave_console(void)
{
  ticks_resume();
}

void ticks_suspend(void)
{
  HAL_SuspendTick();
}
void ticks_resume(void)
{
  HAL_ResumeTick();
}