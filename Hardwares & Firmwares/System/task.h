#ifndef __IVCLOCK_TASK_H__
#define __IVCLOCK_TASK_H__

#include <stdint.h>

// max 32
enum task_events
{
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
  EV_ACC,                         // 有晃动
  EV_TIMER,                       // timer 倒计时结束
  EV_ALARM0,                      // Alarm0响起
  EV_ALARM1,                      // Alarm1响起
  EV_VT1,
  EV_VT2,
  EV_PLAYER_STOP,
  EV_POWEROFF,
  EV_CNT  
};

extern uint16_t ev_bits0;
extern uint16_t ev_bits1;

extern const char * task_names[];

typedef void (*TASK_PROC)(enum task_events);

void task_init (void);

void task_dump(void);

// 这些宏也在中断里被调用，所以不能是带参数函数，只能拿宏实现了
#define task_set(ev1)             \
  do{                             \
    if(ev1 < 16)                  \
      ev_bits0 |= 1<<ev1;         \
    else                          \
      ev_bits1 |= 1<<(ev1 - 16);  \
  }while(0)

#define task_clr(ev1)               \
  do{                               \
    if(ev1 < 16)                    \
      ev_bits0 &= ~(1<<ev1);        \
    else                            \
      ev_bits1 &= ~(1<<(ev1 - 16)); \
  }while(0)
    
#define  task_test(ev1)             \
  (ev1 < 16 ? (ev_bits0 & (1<<ev1)) : (ev_bits1 & (1<<(ev1 - 16))))

void task_run(void);

#endif