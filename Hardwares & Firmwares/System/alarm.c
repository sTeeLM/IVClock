#include "alarm.h"
#include "ds3231.h"
#include "debug.h"
#include "sm.h"
#include "clock.h"
#include "config.h"
#include "player.h"
#include "power.h"

#define ALARM0_MAX_DUR_MIN 30 // 30分钟

static struct alarm0_struct alarm0;

static bool alarm0_is12;
static bool alarm1_enable;

void alarm_proc(enum task_events ev)
{
 
  if(ev == EV_ALARM0) {
    IVDBG("ALARM0 HIT!");
    sm_run(EV_ALARM0);
  }
  
  if(ev == EV_ALARM1) {
    IVDBG("ALARM1 HIT!");
    sm_run(EV_ALARM1);
  } 
}

void alarm_scan(void)
{
  bool alarm0_hit, alarm1_hit;
  
  BSP_DS3231_Read_Data(BSP_DS3231_TYPE_CTL);
  alarm0_hit = BSP_DS3231_Test_Alarm_Int_Flag(BSP_DS3231_ALARM0);
  alarm1_hit = BSP_DS3231_Test_Alarm_Int_Flag(BSP_DS3231_ALARM1);
  BSP_DS3231_Clr_Alarm_Int_Flag(BSP_DS3231_ALARM0);
  BSP_DS3231_Clr_Alarm_Int_Flag(BSP_DS3231_ALARM1);
  BSP_DS3231_Write_Data(BSP_DS3231_TYPE_CTL); 

  if(alarm0_hit) {
    if(alarm0.day_mask & (1 << (clock_get_day() - 1))) {
      task_set(EV_ALARM0);
      if(alarm1_hit) { // 如果同时响起，整点报时被忽略
        alarm1_hit = 0;
      }
    }
  }
  
  // 这段逻辑会被频繁调1S用长达1分钟
  if(alarm1_hit) {
    BSP_DS3231_Read_Data(BSP_DS3231_TYPE_TIME);
    if(BSP_DS3231_Time_Get_Min() == 0 
      && BSP_DS3231_Time_Get_Sec() == 0) {
//      if(power_test_flag()) {
//        power_clr_flag();
//      }
      task_set(EV_ALARM1);
    }
  }
}

void alarm_load_config(void)
{
  alarm0.day_mask = config_read("alm0_day_mask")->val8;
  alarm0.hour = config_read("alm0_hour")->val8;
  alarm0.min  = config_read("alm0_min")->val8;
  alarm0.dur  = config_read("alm0_dur")->val8;
  alarm0_is12 = config_read("time_12")->val8;
  alarm1_enable = config_read("alm1_en")->val8; 
}

void alarm_save_config(enum alarm_sync_type t)
{
  config_val_t val;
  switch (t) {
    case ALARM_SYNC_ALARM0_DAY_MASK:
      val.val8 = alarm0.day_mask;
      config_write("alm0_day_mask", &val);
    break;
    case ALARM_SYNC_ALARM0_HOUR:
      val.val8 = alarm0.hour;
      config_write("alm0_hour", &val);
    break;
    case ALARM_SYNC_ALARM0_MIN:
      val.val8 = alarm0.min;
      config_write("alm0_min", &val);
    break;
    case ALARM_SYNC_ALARM0_DUR:
      val.val8 = alarm0.dur;
      config_write("alm0_dur", &val);
    break;
    case ALARM_SYNC_ALARM1_ENABLE:
      val.val8 = alarm1_enable;
      config_write("alm1_en", &val);
    break;
  }
}



void alarm_init (void)
{
  IVDBG("alarm_init");
  alarm_load_config();
  alarm_dump();
  alarm0_sync_to_rtc();
  alarm1_sync_to_rtc();
  BSP_DS3231_Dump();
}


void alarm_dump(void)
{
  IVDBG("alarm0.day_mask = 0x%02x", alarm0.day_mask);
  IVDBG("alarm0.hour = %u", alarm0.hour);
  IVDBG("alarm0.min  = %u", alarm0.min);
  IVDBG("alarm0.is12 = %s",  alarm0_is12 ? "ON" : "OFF");  
  IVDBG("alarm1.enable = %s",  alarm1_enable ? "ON" : "OFF");  
}


// day  1-7
bool alarm0_test_enable(uint8_t day)
{
  IVDBG("alarm0_test_enable 0x%02x %u!\n", alarm0.day_mask, day);
  return (alarm0.day_mask & (1 << (day - 1))) != 0;
}

// day  1-7
void alarm0_set_enable(uint8_t day, bool enable)
{
  IVDBG("alarm0_set_enable %u %s!\n", day, enable ? "ON" : "OFF");
  day = 1 << (day - 1);
  if(enable) {
    alarm0.day_mask |= day;
  } else {
    alarm0.day_mask &= ~day;
  }
  IVDBG("alarm0_set_enable res 0x%02x!", alarm0.day_mask);
  
}

uint8_t alarm0_get_day_mask(void)
{
  return alarm0.day_mask;
}

bool alarm0_get_hour_12(void)
{
  return alarm0_is12;
}

void alarm0_set_hour_12(bool enable)
{
  alarm0_is12 = enable;
}


uint8_t alarm0_get_min(void)
{
  return alarm0.min;
}

void alarm0_inc_min(void)
{
  alarm0.min = (++ alarm0.min) % 60;
}

uint8_t alarm0_get_hour(void)
{
  return alarm0.hour;
}

void alarm0_inc_hour(void)
{
  alarm0.hour = (++ alarm0.hour) % 24;
}

void alarm0_sync_to_rtc(void)
{
  IVDBG("alarm0_sync_to_rtc!");
  BSP_DS3231_Read_Data(BSP_DS3231_TYPE_ALARM0);
  BSP_DS3231_Alarm_Set_Mode(BSP_DS3231_ALARM0_MOD_MATCH_HOUR_MIN_SEC);
  BSP_DS3231_Alarm_Set_Hour(alarm0.hour);
  BSP_DS3231_Alarm_Set_Min(alarm0.min);
  BSP_DS3231_Alarm_Set_Sec(0);
  BSP_DS3231_Write_Data(BSP_DS3231_TYPE_ALARM0);
  
  BSP_DS3231_Read_Data(BSP_DS3231_TYPE_CTL);
  BSP_DS3231_Enable_Alarm_Int(BSP_DS3231_ALARM0, alarm0.day_mask != 0);
  BSP_DS3231_Clr_Alarm_Int_Flag(BSP_DS3231_ALARM0);
  BSP_DS3231_Write_Data(BSP_DS3231_TYPE_CTL);
}

bool alarm1_test_enable(void)
{
  return alarm1_enable;
}

void alarm1_set_enable(bool enable)
{
  alarm1_enable = enable;
}

void alarm1_sync_to_rtc(void)
{
  IVDBG("alarm1_sync_to_rtc!");
  BSP_DS3231_Read_Data(BSP_DS3231_TYPE_ALARM1);
  BSP_DS3231_Alarm_Set_Mode(BSP_DS3231_ALARM1_MOD_MATCH_MIN);
  BSP_DS3231_Alarm_Set_Hour(0);
  BSP_DS3231_Alarm_Set_Min(0);
  BSP_DS3231_Write_Data(BSP_DS3231_TYPE_ALARM1);
  
  BSP_DS3231_Read_Data(BSP_DS3231_TYPE_CTL);
  BSP_DS3231_Enable_Alarm_Int(BSP_DS3231_ALARM1, alarm1_enable);
  BSP_DS3231_Clr_Alarm_Int_Flag(BSP_DS3231_ALARM1);
  BSP_DS3231_Write_Data(BSP_DS3231_TYPE_CTL);
}

// 5 -> 10 -> 15 -> 20 -> 25 -> 30 -> 5
void alarm0_inc_dur(void)
{
  alarm0.dur += 5;
  if(alarm0.dur > ALARM0_MAX_DUR_MIN) {
    alarm0.dur = 5;
  }
}

uint8_t alarm0_get_dur(void)
{
  return alarm0.dur;
}

void alarm_stop_snd(void)
{
  player_stop_play();
}

void alarm_play_snd(void)
{
  player_play_snd(config_read("alm0_snd")->val8);
}