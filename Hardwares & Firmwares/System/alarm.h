#ifndef __IVCLOCK_ALARM_H__
#define __IVCLOCK_ALARM_H__

#include <stdint.h>
#include "cext.h"
#include "task.h"

void alarm_init(void);
void alarm_proc(enum task_events ev);
void alarm_dump(void);

#define ALARM0_CNT 10

// 闹钟0是一个可以设置日重复的每日闹铃
struct alarm0_struct {
  uint8_t day_mask;                // 1->enable 0->disable
  uint8_t hour;                    // 0-23
  uint8_t min;                     // 0-59
  uint8_t snd;                     // 闹铃声音snd index
};

extern struct alarm0_struct alarm0[ALARM0_CNT];

enum alarm_sync_type {
  ALARM_SYNC_ALARM0,
  ALARM_SYNC_ALARM1
};

uint8_t alarm0_get_cnt(void);
uint8_t alarm0_get_hit_index(void);
void alarm_save_config(enum alarm_sync_type t, uint8_t index);
void alarm_load_config(void);
void alarm_resync_rtc(void);

bool alarm0_test_enable(uint8_t index, uint8_t day);
void alarm0_set_enable(uint8_t index, uint8_t day, bool enable);
uint8_t alarm0_get_day_mask(uint8_t index);
uint8_t alarm0_get_min(uint8_t index);
void alarm0_inc_min(uint8_t index);
uint8_t alarm0_get_hour(uint8_t index);
void alarm0_inc_hour(uint8_t index);

// 闹铃铃声
void alarm0_inc_snd(uint8_t index);
uint8_t alarm0_get_snd(uint8_t index);


// 闹钟1是整点报时闹钟
bool alarm1_test_enable(void);
void alarm1_set_enable(bool enable);

void alarm_scan(void);
void alarm_show(void);

void alarm0_stop_snd(void);
void alarm0_play_snd(uint8_t index);
void alarm1_stop_snd(void);
void alarm1_play_snd(void);

#endif