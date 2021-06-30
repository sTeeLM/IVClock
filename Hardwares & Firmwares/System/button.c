#include "button.h"
#include "bsp.h"
#include "key.h"
#include "delay.h"
#include "cext.h"
#include "task.h"
#include "clock.h"
#include "debug.h"
#include "sm.h"
#include "beeper.h"

#define KEY_PRESS_DELAY 200 // 防止抖动，检测延迟时间，200 us
#define KEY_LPRESS_DELAY 3 // 长按时间，3s
#define KEY_2_KEY_LPRESS_DELAY 2 // 同时按下长按时间，2s

static uint8_t last_mod_tmr_count; 
static uint8_t last_set_tmr_count;
static uint8_t mod_press;
static uint8_t set_press;

bool button_init(void)
{
  last_mod_tmr_count = 0;
  last_set_tmr_count = 0;
   
  mod_press = 0; 
  set_press = 0;
  return 0;
}

void button_scan_proc(enum task_events ev)
{
  UNUSED(ev);
  if(BSP_Key_Set_Pressed() && !set_press) {
    delay_us(KEY_PRESS_DELAY);
    if(BSP_Key_Set_Pressed()) {
      task_set(EV_BUTTON_SET_DOWN);      
      set_press = 1;
      last_set_tmr_count = clock_get_now_sec();     
    }
  } else if(BSP_Key_Set_Pressed() && set_press){
    if(clock_diff_now_sec(last_set_tmr_count) >= KEY_LPRESS_DELAY) {
      task_set(EV_BUTTON_SET_LPRESS);
    }
  } else if(!BSP_Key_Set_Pressed() && set_press) {
    task_set(EV_BUTTON_SET_UP);
    set_press = 0;
    if(clock_diff_now_sec(last_set_tmr_count) < KEY_LPRESS_DELAY) {
      task_set(EV_BUTTON_SET_PRESS);
      if(mod_press) {
        task_set(EV_BUTTON_MOD_SET_PRESS);    
      }         
    }    
  }
  
  if(BSP_Key_Mod_Pressed() && !mod_press) {
    delay_us(KEY_PRESS_DELAY);
    if(BSP_Key_Mod_Pressed()) {
      task_set(EV_BUTTON_MOD_DOWN);     
      mod_press = 1;
      last_mod_tmr_count = clock_get_now_sec();       
    }
  } else if(BSP_Key_Mod_Pressed() && mod_press){
    if(clock_diff_now_sec(last_mod_tmr_count) >= KEY_LPRESS_DELAY) {
      task_set(EV_BUTTON_MOD_LPRESS);
    }
  } else if(!BSP_Key_Mod_Pressed() && mod_press) {
    task_set(EV_BUTTON_MOD_UP);
    if(clock_diff_now_sec(last_mod_tmr_count) < KEY_LPRESS_DELAY) {
      task_set(EV_BUTTON_MOD_PRESS);
      if(set_press) {
        task_set(EV_BUTTON_SET_PRESS);    
      }        
    }
    mod_press = 0;
  }
  
  if(BSP_Key_Set_Pressed() && set_press 
    && BSP_Key_Mod_Pressed() && mod_press) {
    if(clock_diff_now_sec(last_mod_tmr_count) >= KEY_2_KEY_LPRESS_DELAY
      && clock_diff_now_sec(last_set_tmr_count) >= KEY_2_KEY_LPRESS_DELAY) {
      task_set(EV_BUTTON_MOD_SET_LPRESS);
    }
  }
}

void button_mod_proc(enum task_events ev)
{
  switch (ev) {
    case EV_BUTTON_MOD_DOWN:
      IVDBG("button_mod_proc EV_BUTTON_MOD_DOWN");
      beeper_beep();
      break;
    case EV_BUTTON_MOD_UP:
      IVDBG("button_mod_proc EV_BUTTON_MOD_UP\n");
      break; 
    case EV_BUTTON_MOD_PRESS:
      IVDBG("button_mod_proc EV_BUTTON_MOD_PRESS\n");
      break;
    case EV_BUTTON_MOD_LPRESS:
      IVDBG("button_mod_proc EV_BUTTON_MOD_LPRESS\n");
      break; 
		default:
			;
  }
  
  sm_run(ev);
}

void button_set_proc(enum task_events ev)
{
  switch (ev) {
    case EV_BUTTON_SET_DOWN:
      IVDBG("button_set_proc EV_BUTTON_SET_DOWN");
      beeper_beep();
      break;
    case EV_BUTTON_SET_UP:
      IVDBG("button_set_proc EV_BUTTON_SET_UP\n");
      break; 
    case EV_BUTTON_SET_PRESS:
      IVDBG("button_set_proc EV_BUTTON_SET_PRESS\n");
      break;
    case EV_BUTTON_SET_LPRESS:
      IVDBG("button_set_proc EV_BUTTON_SET_LPRESS\n");
      break; 
		default:
			;
  }
  
  sm_run(ev);
}


void button_mod_set_proc(enum task_events ev)
{
  switch (ev) {
    case EV_BUTTON_MOD_SET_PRESS:
      IVDBG("button_mod_set_proc EV_BUTTON_MOD_SET_PRESS\n");
      break;
    case EV_BUTTON_MOD_SET_LPRESS:
      IVDBG("button_mod_set_proc EV_BUTTON_MOD_SET_LPRESS\n");
      break;
		default:
			;    
  }

  sm_run(ev);
}
