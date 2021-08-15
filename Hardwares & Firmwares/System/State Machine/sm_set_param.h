#ifndef __IVCLOCK_SM_SET_PARAM_H__
#define __IVCLOCK_SM_SET_PARAM_H__

#include "task.h"

extern const char * sm_states_names_set_param[];
extern struct sm_trans * sm_trans_set_param[];

enum sm_states_set_param
{
  SM_SET_PARAM_INIT,
  SM_SET_PARAM_BEEPER,     // beeper声音
  SM_SET_PARAM_BAOSHI,     // 整点报时
  SM_SET_PARAM_TIMER_SND,  // 计时器铃声 
  SM_SET_PARAM_PS,         // 省电设置
  SM_SET_PARAM_PLAYER_VOL, // player音量
  SM_SET_PARAM_HOUR12,     // 12/24显示格式
  SM_SET_PARAM_LIGHT_MON,  // 光照亮度调整
  SM_SET_PARAM_MOTION_MON, // 震动唤醒功能和震动敏感度

};

void do_set_param_init(uint8_t from_func, uint8_t from_state, uint8_t to_func, uint8_t to_state, enum task_events ev);

#endif
