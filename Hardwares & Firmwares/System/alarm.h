#ifndef __IVCLOCK_ALARM_H__
#define __IVCLOCK_ALARM_H__

#include <stdint.h>
#include "cext.h"
#include "task.h"

void alarm_init(void);
void alarm_proc(enum task_events ev);
void alarm_dump(void);


// 闹钟0是一个可以设置日重复的每日闹铃
struct alarm0_struct {
  uint8_t day_mask;                // 1->enable 0->disable
  uint8_t hour;                    // 0-23
  uint8_t min;                     // 0-59
  uint8_t snd;                     // 闹铃声音snd index
};

extern struct alarm0_struct alarm0;

enum alarm_sync_type {
  ALARM_SYNC_ALARM0_DAY_MASK,
  ALARM_SYNC_ALARM0_HOUR,
  ALARM_SYNC_ALARM0_MIN,
  ALARM_SYNC_ALARM0_SND,
  ALARM_SYNC_ALARM1_ENABLE
};

void alarm_save_config(enum alarm_sync_type t);
void alarm_load_config(void);
bool alarm0_test_enable(uint8_t day);
void alarm0_set_enable(uint8_t day, bool enable);
uint8_t alarm0_get_day_mask(void);
bool alarm0_get_hour_12(void);
void alarm0_set_hour_12(bool enable);
uint8_t alarm0_get_min(void);
void alarm0_inc_min(void);
uint8_t alarm0_get_hour(void);
void alarm0_inc_hour(void);
//void alarm0_sync_from_rtc(void);
void alarm0_sync_to_rtc(void);
// 闹铃铃声
void alarm0_inc_snd(void);
uint8_t alarm0_get_snd(void);


// 闹钟1是整点报时闹钟
bool alarm1_test_enable(void);
void alarm1_set_enable(bool enable);
//void alarm1_sync_from_rtc(void);
void alarm1_sync_to_rtc(void);
void alarm_scan(void);

void alarm0_stop_snd(void);
void alarm0_play_snd(void);
void alarm1_stop_snd(void);
void alarm1_play_snd(void);

#endif