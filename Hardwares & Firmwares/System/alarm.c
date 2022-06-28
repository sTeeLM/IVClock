#include <string.h>
#include "alarm.h"
#include "ds3231.h"
#include "debug.h"
#include "sm.h"
#include "clock.h"
#include "config.h"
#include "player.h"
#include "power.h"
#include "console.h"
#include "rtc.h"

#define ALARM0_CNT 10

static int8_t alarm0_cur; // -1说明当前rtc中没有alarm0生效
static uint8_t alarm0_hit_index;
static struct alarm0_struct alarm0[ALARM0_CNT];
static uint8_t alarm1_begin;
static uint8_t alarm1_end;

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

static void alarm0_sync_to_rtc(int8_t index)
{
  IVDBG("alarm0_sync_to_rtc %d!", index);
  if(index >= 0) {
    BSP_DS3231_Read_Data(BSP_DS3231_TYPE_ALARM0);
    BSP_DS3231_Alarm_Set_Mode(BSP_DS3231_ALARM0, BSP_DS3231_ALARM0_MOD_MATCH_HOUR_MIN_SEC);
    BSP_DS3231_Alarm_Set_Hour(BSP_DS3231_ALARM0, alarm0[index].hour);
    BSP_DS3231_Alarm_Set_Min(BSP_DS3231_ALARM0, alarm0[index].min);
    BSP_DS3231_Alarm_Set_Sec(BSP_DS3231_ALARM0, 0);
    BSP_DS3231_Write_Data(BSP_DS3231_TYPE_ALARM0);
    
    BSP_DS3231_Read_Data(BSP_DS3231_TYPE_CTL);
    BSP_DS3231_Enable_Alarm_Int(BSP_DS3231_ALARM0, TRUE);
    BSP_DS3231_Clr_Alarm_Int_Flag(BSP_DS3231_ALARM0);
    BSP_DS3231_Write_Data(BSP_DS3231_TYPE_CTL);
  } else {
    BSP_DS3231_Read_Data(BSP_DS3231_TYPE_CTL);
    BSP_DS3231_Enable_Alarm_Int(BSP_DS3231_ALARM0, FALSE);
    BSP_DS3231_Clr_Alarm_Int_Flag(BSP_DS3231_ALARM0);
    BSP_DS3231_Write_Data(BSP_DS3231_TYPE_CTL);
  }
}

static void alarm1_sync_to_rtc(void)
{
  IVDBG("alarm1_sync_to_rtc!");
  BSP_DS3231_Read_Data(BSP_DS3231_TYPE_ALARM1);
  BSP_DS3231_Alarm_Set_Mode(BSP_DS3231_ALARM1, BSP_DS3231_ALARM1_MOD_MATCH_MIN);
  BSP_DS3231_Alarm_Set_Hour(BSP_DS3231_ALARM1, 0);
  BSP_DS3231_Alarm_Set_Min(BSP_DS3231_ALARM1, 0);
  BSP_DS3231_Write_Data(BSP_DS3231_TYPE_ALARM1);
  
  BSP_DS3231_Read_Data(BSP_DS3231_TYPE_CTL);
  BSP_DS3231_Enable_Alarm_Int(BSP_DS3231_ALARM1, alarm1_enable);
  BSP_DS3231_Clr_Alarm_Int_Flag(BSP_DS3231_ALARM1);
  BSP_DS3231_Write_Data(BSP_DS3231_TYPE_CTL);
}

/*
void  alarm_bubble_sort(uint8_t * arr, uint8_t len) 
{   
  uint8_t i, j, temp;

  for(i = 0; i < len - 1; i++) {
    for(j = 0; j < len - 1 - i; j++) {
      // 相邻元素两两对比
      if(alarm0[arr[j]].hour > alarm0[arr[j+1]].hour 
        || (alarm0[arr[j]].hour == alarm0[arr[j+1]].hour 
        && alarm0[arr[j]].min > alarm0[arr[j+1]].min)) {
        temp = arr[j+1];            // 元素交换
        arr[j+1] = arr[j];
        arr[j] = temp;
      }
    }
  }
}
*/

// 结合当前clock时间，确定哪一个alarm0应该放在rtc里
// 这个在alarm触发以及时钟时间修改的时候都应该被更新
static char alarm0_hit_map[7][1440]; // 1440 = 24 * 60
static int8_t alarm0_find_curr(void)
{
  uint8_t index[ALARM0_CNT];
  uint8_t i, day, hour, min;
  uint16_t j, cur_pos;
  
  memset(alarm0_hit_map, 0, sizeof(alarm0_hit_map));
  
  for(i = 0 ; i < ALARM0_CNT ; i++){
    for(day = 1 ; day <= 7 ; day ++) {
      if(alarm0_test_enable(i, day)) {
        alarm0_hit_map[day - 1][alarm0[i].hour * 24 + alarm0[i].min] = i + 1;
      }
    }
  }
  
  hour = clock_get_hour();
  min  = clock_get_min();
  day  = clock_get_day();

  day = day - 1; // 1-7 -> 0-6
  
  // 从alarm0_hit_map的当前位置开始，向后找第一个
  cur_pos = day * 1440 + hour * 24 + min;
  
  for(j = (cur_pos + 1); j < 1440 * 7; j ++) {
    if(alarm0_hit_map[j / 1440][j % 1440]) {
      IVDBG("alarm0_find_curr return %d", alarm0_hit_map[j / 1440][j % 1440] - 1);
      return alarm0_hit_map[j / 1440][j % 1440] - 1;      
    }
  }

  // 没找到，到头绕过开始，不排除又找到同一个位置的自己了
  for(j = 0 ; j <= cur_pos ;j ++) {
    if(alarm0_hit_map[j / 1440][j % 1440]) {
      IVDBG("alarm0_find_curr return %d", alarm0_hit_map[j / 1440][j % 1440] - 1);
      return alarm0_hit_map[j / 1440][j % 1440] - 1;      
    }    
  }
  IVDBG("alarm0_find_curr return %d", -1);
  return -1;
}

void alarm_scan(void)
{
  bool alarm0_hit, alarm1_hit;
  uint8_t year, mon, date, day, hour, min, sec;
  BSP_DS3231_Read_Data(BSP_DS3231_TYPE_CTL);
  alarm0_hit = BSP_DS3231_Test_Alarm_Int_Flag(BSP_DS3231_ALARM0);
  alarm1_hit = BSP_DS3231_Test_Alarm_Int_Flag(BSP_DS3231_ALARM1);
  BSP_DS3231_Clr_Alarm_Int_Flag(BSP_DS3231_ALARM0);
  BSP_DS3231_Clr_Alarm_Int_Flag(BSP_DS3231_ALARM1);
  BSP_DS3231_Write_Data(BSP_DS3231_TYPE_CTL); 

  if(alarm0_hit) {
    if(alarm0_cur >= 0) {
      rtc_get_date(&year, &mon, &date, &day);
      if(alarm0[alarm0_cur].day_mask & (1 << (day - 1))) {
        power_wakeup();
        alarm0_hit_index = alarm0_cur;
        task_set(EV_ALARM0);
        if(alarm1_hit) { // 如果同时响起，整点报时被忽略
          alarm1_hit = 0;
        }
      }
    }
    // 尝试在rtc里设置下一个时钟
    alarm0_cur = alarm0_find_curr();
    alarm0_sync_to_rtc(alarm0_cur);
    alarm_dump();
  }
  
  // 这段逻辑会被频繁调1S用长达1分钟
  if(alarm1_hit) {
    rtc_get_time(&hour, &min, &sec);
    if(min == 0 && sec == 0) {
      IVDBG("alarm1 hour = %d, begin = %d, end = %d", hour, alarm1_begin, alarm1_end);
      if(alarm1_begin < alarm1_end && (hour >= alarm1_begin && hour <= alarm1_end)
        || alarm1_begin >= alarm1_end && (hour >= alarm1_end && hour <= alarm1_begin)) {
          IVDBG("alarm1 really hit!");
          power_wakeup();
          task_set(EV_ALARM1);
        }
    }
  }
}

static char alarm0_name[] = {'a', 'l', 'm', '0', 0, '_', 'c', 'f', 'g', 0};

void alarm_load_config(void)
{
  uint8_t i;
  config_val_t val;
  for(i = 0 ; i < ALARM0_CNT ; i ++) {
    val.valblob.body = (uint8_t *) &alarm0[i];
    alarm0_name[4] = i + 0x30;
    config_read(alarm0_name, &val);
  }
  alarm1_enable = config_read_int("alm1_en"); 
  alarm0_sync_to_rtc((alarm0_cur = alarm0_find_curr()));
  alarm1_sync_to_rtc();  
  alarm1_begin = config_read_int("alm1_begin");
  alarm1_end = config_read_int("alm1_end");  
}

void alarm_resync_rtc(void)
{
  alarm0_sync_to_rtc((alarm0_cur = alarm0_find_curr()));
  alarm_dump();
}

uint8_t alarm0_get_cnt(void)
{
  return ALARM0_CNT;
}

void alarm_save_config(enum alarm_sync_type t, uint8_t index)
{
  config_val_t val;
  switch (t) {
    case ALARM_SYNC_ALARM0:
      val.valblob.len  = sizeof(struct alarm0_struct);
      val.valblob.body = (uint8_t*)&alarm0[index];
      alarm0_name[4] = index + 0x30;
      config_write(alarm0_name, &val);
      alarm0_sync_to_rtc((alarm0_cur = alarm0_find_curr()));
    break;
    case ALARM_SYNC_ALARM1:
      val.val8 = alarm1_enable;
      config_write("alm1_en", &val);
      val.val8 = alarm1_begin;
      config_write("alm1_begin", &val);
      val.val8 = alarm1_end;
      config_write("alm1_end", &val);   
      alarm1_sync_to_rtc();
    break;
  }
}

void alarm_init (void)
{
  IVDBG("alarm_init");
  alarm_load_config();
  alarm_dump();
  BSP_DS3231_Dump();
  alarm0_hit_index = 0;
}

uint8_t alarm0_get_hit_index(void)
{
  return alarm0_hit_index;
}

void alarm_dump(void)
{
  uint8_t i;
  IVDBG("----------- alarm dump begin -----------------");
  for(i = 0 ; i < ALARM0_CNT ; i++) {
    IVDBG("alarm0[%d].day_mask = 0x%02x", i, alarm0[i].day_mask);
    IVDBG("alarm0[%d].hour = %d", i, alarm0[i].hour);
    IVDBG("alarm0[%d].min  = %d", i, alarm0[i].min);
    IVDBG("alarm0[%d].snd  = %d", i, alarm0[i].snd);
  }
  IVDBG("current actived alarm0: %d", alarm0_cur);
  IVDBG("current alarm0_hit_index: %d", alarm0_hit_index); 
  IVDBG("alarm1.enable = %s",  alarm1_enable ? "ON" : "OFF"); 
  IVDBG("alarm1.begin = %d",  alarm1_begin);
  IVDBG("alarm1.end = %d",  alarm1_end);  
  IVDBG("----------- alarm dump end -----------------");  
}

void alarm_show(void)
{
  uint8_t i;
  for(i = 0 ; i < ALARM0_CNT ; i++) {
    console_printf("alarm0[%d].day_mask = 0x%02x\r\n", i, alarm0[i].day_mask);
    console_printf("alarm0[%d].hour = %d\r\n", i, alarm0[i].hour);
    console_printf("alarm0[%d].min  = %d\r\n", i, alarm0[i].min);
    console_printf("alarm0[%d].snd  = %d\r\n", i, alarm0[i].snd);
  }
  console_printf("current actived alarm0: %d\r\n", alarm0_cur);
  console_printf("current alarm0_hit_index: %d\r\n", alarm0_hit_index); 
  console_printf("alarm1.enable = %s\r\n",  alarm1_enable ? "ON" : "OFF"); 
  console_printf("alarm1.begin = %d\r\n",  alarm1_begin);
  console_printf("alarm1.end = %d\r\n",  alarm1_end); 
}

// day  1-7
bool alarm0_test_enable(uint8_t index, uint8_t day)
{
  //IVDBG("alarm0_test_enable %d 0x%02x %u!", index, alarm0[index].day_mask, day);
  return (alarm0[index].day_mask & (1 << (day - 1))) != 0;
}

// day  1-7
void alarm0_set_enable(uint8_t index, uint8_t day, bool enable)
{
  IVDBG("alarm0_set_enable %d %u %s!", index, day, enable ? "ON" : "OFF");
  day = 1 << (day - 1);
  if(enable) {
    alarm0[index].day_mask |= day;
  } else {
    alarm0[index].day_mask &= ~day;
  }
  IVDBG("alarm0_set_enable res %d 0x%02x!", index, alarm0[index].day_mask);
}

uint8_t alarm0_get_day_mask(uint8_t index)
{
  return alarm0[index].day_mask;
}

void alarm0_set_day_mask(uint8_t index, uint8_t daymask)
{
  alarm0[index].day_mask = (daymask & 0x7F);
}

uint8_t alarm0_get_min(uint8_t index)
{
  return alarm0[index].min;
}
void alarm0_set_min(uint8_t index, uint8_t min)
{
  alarm0[index].min = min % 60;
}

void alarm0_inc_min(uint8_t index)
{
  alarm0[index].min = (++ alarm0[index].min) % 60;
}

uint8_t alarm0_get_hour(uint8_t index)
{
  return alarm0[index].hour;
}

void alarm0_set_hour(uint8_t index, uint8_t hour)
{
  alarm0[index].hour = hour % 24;
}

void alarm0_inc_hour(uint8_t index)
{
  alarm0[index].hour = (++ alarm0[index].hour) % 24;
}

void alarm0_inc_snd(uint8_t index)
{
  alarm0[index].snd ++;
  if(alarm0[index].snd >= player_get_snd_cnt(PLAYER_SND_DIR_ALARM)) {
    alarm0[index].snd = 0;
  }
}

uint8_t alarm0_get_snd(uint8_t index)
{
  return alarm0[index].snd;
}

void alarm0_set_snd(uint8_t index, uint8_t snd)
{
  alarm0[index].snd = snd;
  if(alarm0[index].snd >= player_get_snd_cnt(PLAYER_SND_DIR_ALARM)) {
    alarm0[index].snd = 0;
  }
}

void alarm0_stop_snd(void)
{
  if(player_is_playing())
    player_stop_play();
}

void alarm0_play_snd(uint8_t index)
{
  player_play_snd(PLAYER_SND_DIR_ALARM, alarm0[index].snd);
}

bool alarm1_test_enable(void)
{
  return alarm1_enable;
}

void alarm1_set_enable(bool enable)
{
  alarm1_enable = enable;
}

uint8_t alarm1_get_begin(void)
{
  return alarm1_begin;
}

void alarm1_inc_begin(void)
{
  alarm1_begin ++;
  alarm1_begin %= 24;
}

void alarm1_set_begin(uint8_t begin)
{
  alarm1_begin = begin;
  alarm1_begin %= 24;
}

uint8_t alarm1_get_end(void)
{
  return alarm1_end;
}

void alarm1_inc_end(void)
{
  alarm1_end ++;
  alarm1_end %= 24;
}

void alarm1_set_end(uint8_t end)
{
  alarm1_end = end;
  alarm1_end %= 24;
}

void alarm1_stop_snd(void)
{
  if(player_is_playing())
    player_stop_play();
}

void alarm1_play_snd(void)
{
  player_report_clk_and_temp();
}