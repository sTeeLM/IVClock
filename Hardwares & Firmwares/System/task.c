#include <stdio.h>
#include <string.h>

#include "task.h"
#include "debug.h"
#include "sm.h"

/* hardware*/
#include "clock.h"
#include "button.h"
#include "alarm.h"
#include "console.h"

/*
  EV_250MS            = 0, // 大约每250ms转一下
  EV_250MS            = 0, // 大约每250ms转一下
  EV_1S               = 1, // 大约每1s转一下  
  EV_BUTTON_SCAN         = 2, // 扫描按键 
  EV_BUTTON_SET_DOWN     = 3, // set键按下
  EV_BUTTON_SET_PRESS    = 4, // set键短按
  EV_BUTTON_SET_LPRESS   = 5, // set键长按
  EV_BUTTON_SET_UP       = 6, // set键抬起
  EV_BUTTON_MOD_DOWN     = 7, // mod键按下
  EV_BUTTON_MOD_PRESS    = 8, // mod键短按
  EV_BUTTON_MOD_LPRESS   = 9, // mod键长按
  EV_BUTTON_MOD_UP       = 10, // mod键抬起 
  EV_BUTTON_MOD_SET_PRESS    = 11, // mod set 键同时短按
  EV_BUTTON_MOD_SET_LPRESS   = 12, // mod set 键同时长按 

  EV_COUNT  
*/
const char * task_names[] =
{
  "EV_250MS",
  "EV_1S",
  "EV_BUTTON_SCAN",
  "EV_BUTTON_SET_DOWN",
  "EV_BUTTON_SET_PRESS",
  "EV_BUTTON_SET_LPRESS",
  "EV_BUTTON_SET_UP",
  "EV_BUTTON_MOD_DOWN",
  "EV_BUTTON_MOD_PRESS",
  "EV_BUTTON_MOD_LPRESS",
  "EV_BUTTON_MOD_UP",
  "EV_BUTTON_MOD_SET_PRESS",
  "EV_BUTTON_MOD_SET_LPRESS",
	"EV_ACC",
	"EV_TIMER",
	"EV_ALARM0",
	"EV_ALARM1",	
};

static void null_proc(enum task_events ev)
{
	sm_run(ev);
}


static const TASK_PROC task_procs[EV_CNT] = 
{
  /* EV_250MS            = 0, // 大约每250ms转一下 */
  clock_time_proc,
  /* EV_1S               = 1, // 大约每1s转一下   */
  clock_time_proc,
  /* EV_SCAN_KEY         = 2, // 扫描按键 */
  button_scan_proc,
  /* EV_KEY_SET_XX */
  button_set_proc,
  button_set_proc,
  button_set_proc,
  button_set_proc,
  /* EV_KEY_MOD_XX */
  button_mod_proc,
  button_mod_proc,
  button_mod_proc,
  button_mod_proc,
  /* EV_KEY_MOD_SET_XX */
  button_mod_set_proc,
  button_mod_set_proc, 
	null_proc,
	null_proc,
	alarm_proc,
	alarm_proc,  
};


uint16_t ev_bits0;
uint16_t ev_bits1;

void task_init (void)
{
  ev_bits0 = 0;
  ev_bits1 = 0;
}

void task_run(void)
{
  unsigned char c;

  while((c = console_try_get_key()) != 'c' || c == 0) {
    for(c = 0; c < EV_CNT; c++) {
      if(task_test(c)) {
        task_clr(c);
        task_procs[c](c);
      }
    }
  }
  
}

void task_dump(void)
{
  unsigned char i;
  for (i = 0 ; i < EV_CNT; i ++) {
    IVDBG("[%02bd][%s] %c\n", i, task_names[i], task_test(i) ? '1' : '0');
  }
}
