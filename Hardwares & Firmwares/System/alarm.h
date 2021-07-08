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
  uint8_t dur;                     // 闹铃持续时间，分钟
};

enum alarm_sync_type {
  ALARM_SYNC_ALARM0_DAY_MASK,
  ALARM_SYNC_ALARM0_HOUR,
  ALARM_SYNC_ALARM0_MIN,
  ALARM_SYNC_ALARM0_DUR,
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
// 闹铃持续时间
void alarm0_inc_dur(void);
uint8_t alarm0_get_dur(void);


// 闹钟1是整点报时闹钟
bool alarm1_test_enable(void);
void alarm1_set_enable(bool enable);
//void alarm1_sync_from_rtc(void);
void alarm1_sync_to_rtc(void);
void alarm_scan(void);

void alarm_stop_mp3(void);
void alarm_play_mp3(void);

#endif