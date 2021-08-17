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

#define ALARM0_MAX_SND_INDEX 9

static int8_t alarm0_cur; // -1说明当前rtc中没有alarm0生效
static uint8_t alarm0_hit_index;
struct alarm0_struct alarm0[ALARM0_CNT];

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
    BSP_DS3231_Alarm_Set_Mode(BSP_DS3231_ALARM0_MOD_MATCH_HOUR_MIN_SEC);
    BSP_DS3231_Alarm_Set_Hour(alarm0[index].hour);
    BSP_DS3231_Alarm_Set_Min(alarm0[index].min);
    BSP_DS3231_Alarm_Set_Sec(0);
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
  BSP_DS3231_Alarm_Set_Mode(BSP_DS3231_ALARM1_MOD_MATCH_MIN);
  BSP_DS3231_Alarm_Set_Hour(0);
  BSP_DS3231_Alarm_Set_Min(0);
  BSP_DS3231_Write_Data(BSP_DS3231_TYPE_ALARM1);
  
  BSP_DS3231_Read_Data(BSP_DS3231_TYPE_CTL);
  BSP_DS3231_Enable_Alarm_Int(BSP_DS3231_ALARM1, alarm1_enable);
  BSP_DS3231_Clr_Alarm_Int_Flag(BSP_DS3231_ALARM1);
  BSP_DS3231_Write_Data(BSP_DS3231_TYPE_CTL);
}


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

// 结合当前clock时间，确定哪一个alarm0应该放在rtc里
// 这个在alarm触发以及时钟时间修改的时候都应该被更新
static int8_t alarm0_find_curr(void)
{
  uint8_t index[ALARM0_CNT];
  uint8_t i, hour, min, sec;
  for(i = 0 ; i < ALARM0_CNT ; i++){
    index[i] = i;
  }
  // 对alarm从早到晚排序
  alarm_bubble_sort(index, ALARM0_CNT);
  IVDBG("alarm_bubble_sort:");
  IVDBG_RH;
  for(i = 0 ; i < ALARM0_CNT; i ++) {
    IVDBG_R(" %d", index[i]);
  }
  IVDBG_RT;
  
  rtc_get_time(&hour, &min, &sec);
  
  // 寻找第一个晚于当前时间的alarm0 slot
  // 如果没有就从前朝后找第一个有效的
  for(i = 0 ; i < ALARM0_CNT ; i ++) {
    if((alarm0[index[i]].hour >  hour 
      || (alarm0[index[i]].hour  ==  hour && alarm0[index[i]].min > min))
      && alarm0[index[i]].day_mask) {
      IVDBG("alarm0_find_curr return %d", index[i]);
      return index[i];
    }
  }
  
  for(i = 0 ; i < ALARM0_CNT ; i ++) {
    if(alarm0[index[i]].day_mask) {
      IVDBG("alarm0_find_curr return %d", index[i]);
      return index[i];
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
      power_wakeup();
      task_set(EV_ALARM1);
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
}

// day  1-7
bool alarm0_test_enable(uint8_t index, uint8_t day)
{
  IVDBG("alarm0_test_enable %d 0x%02x %u!", index, alarm0[index].day_mask, day);
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

uint8_t alarm0_get_min(uint8_t index)
{
  return alarm0[index].min;
}

void alarm0_inc_min(uint8_t index)
{
  alarm0[index].min = (++ alarm0[index].min) % 60;
}

uint8_t alarm0_get_hour(uint8_t index)
{
  return alarm0[index].hour;
}

void alarm0_inc_hour(uint8_t index)
{
  alarm0[index].hour = (++ alarm0[index].hour) % 24;
}

void alarm0_inc_snd(uint8_t index)
{
  config_val_t val;
  alarm0[index].snd ++;
  if(alarm0[index].snd > ALARM0_MAX_SND_INDEX) {
    alarm0[index].snd = 0;
  }
  val.val8 = alarm0[index].snd;
}

uint8_t alarm0_get_snd(uint8_t index)
{
  return alarm0[index].snd;
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

void alarm1_stop_snd(void)
{
  if(player_is_playing())
    player_stop_play();
}

void alarm1_play_snd(void)
{
  player_report_clk();
}