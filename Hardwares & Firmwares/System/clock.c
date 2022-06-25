#include "clock.h"
#include "cext.h"
#include "display.h"
#include "rtc.h"
#include "debug.h"
#include "config.h"
#include "sm.h"
#include "console.h"
#include "display.h"
#include "button.h"
#include "key.h"
#include "tim.h"
#include "alarm.h"

#include<string.h>


#define CLOCK_FACTORY_RESET_HOUR 12
#define CLOCK_FACTORY_RESET_MIN  11
#define CLOCK_FACTORY_RESET_SEC  0
#define CLOCK_FACTORY_RESET_CENTRY 1
#define CLOCK_FACTORY_RESET_YEAR 14
#define CLOCK_FACTORY_RESET_MON  8
#define CLOCK_FACTORY_RESET_DATA 19
#define CLOCK_FACTORY_RESET_DAY  2
      

static uint8_t date_table[2][12] = 
{
{31,29,31,30,31,30,31,31,30,31,30,31,}, // 2000
{31,28,31,30,31,30,31,31,30,31,30,31,},
};

struct clock_struct clk;
static struct clock_struct saved_clk;
static uint32_t now_sec; // 用于 time_diff
static bool clock_is_hour12;
static bool refresh_display;

void clock_refresh_display_enable(bool enable)
{
  refresh_display = enable;
}

bool clock_test_hour12(void)
{
  return clock_is_hour12;
}

void clock_set_hour12(bool enable)
{
  clock_is_hour12 = enable;
}

void clock_save_config(void)
{
  config_val_t val;
  val.val8 = clock_is_hour12;
    config_write("time_12", &val);
}

//
// 在调整了year，mon，date之后都得调用
//
static void clock_recal_date_day(void)
{
  // 如果是2月并且不是闰年，需要保证不能出现2月29日
  // 以及不能出现4月31日这样的情况
  if(clk.date > clock_get_mon_date(clk.year, clk.mon) - 1)
    clk.date = clock_get_mon_date(clk.year, clk.mon) - 1;
  clk.day = cext_yymmdd_to_day(clk.year, clk.mon, clk.date);
}

// 1 / 256 = 0.00390625
void clock_inc_ms39(void)
{
  int16_t y;
  clk.ms39 ++;
  
  if(clk.ms39 == 0 ) {
    clk.sec = (++ clk.sec) % 60;
    now_sec ++;

    if(clk.sec == 0) {
      clk.min = (++ clk.min) % 60;
      if(clk.min == 0) {
        clk.hour = (++ clk.hour) % 24;
        if(clk.hour == 0) {
          if(is_leap_year(clk.year)) {
            clk.date = (++ clk.date) % date_table[0][clk.mon];
          } else {
            clk.date = (++ clk.date) % date_table[1][clk.mon];
          }
          clk.day = (++ clk.day) % 7;
          if(clk.date == 0) {
            clk.mon = (++ clk.mon) % 12;
            if(clk.mon == 0) {
              clk.year = (++ clk.year);
              if(clk.year > 2099) {
                clk.year = 1901; // 看不到了吧，这个点我都120岁了，灰都没有了
                clock_recal_date_day();
                clock_enable_interrupt(FALSE);
                clock_sync_to_rtc(CLOCK_SYNC_DATE); 
                clock_enable_interrupt(TRUE);
                alarm_resync_rtc();
              }
            }
          }
        }
      }
    } 
    
    clock_refresh_display();
  }

}

void clock_refresh_display(void)
{
  if(refresh_display && display_is_on()) {
    display_format_clock(&clk);
  }
}

void clock_show(void)
{
  console_printf("%04d-%02d-%02d %02d:%02d:%02d:%03d\r\n",
    clk.year, clk.mon + 1, clk.date + 1,
    clk.hour, clk.min, clk.sec, clk.ms39);
}

void clock_dump(void)
{
  IVDBG("clk.year = %u", clk.year);
  IVDBG("clk.mon  = %u", clk.mon);
  IVDBG("clk.date = %u", clk.date); 
  IVDBG("clk.day  = %u", clk.day);
  IVDBG("clk.hour = %u", clk.hour); 
  IVDBG("clk.min  = %u", clk.min);
  IVDBG("clk.sec  = %u", clk.sec);  
  IVDBG("clk.ms39 = %u", clk.ms39); 
}


uint8_t clock_get_ms39(void)
{
  return clk.ms39;
}

uint32_t clock_get_now_sec(void)
{
  return now_sec;
}


uint32_t clock_diff_now_sec(uint32_t sec)
{
  return (uint32_t)(now_sec - sec);
}

uint8_t clock_get_sec(void)
{
  return clk.sec;
}

void clock_set_sec(uint8_t sec)
{
  clk.sec = sec % 60;
}

void clock_clr_sec(void)
{
   clk.sec = 0;
}

uint8_t clock_get_min(void)
{
  return clk.min;
}

void clock_set_min(uint8_t min)
{
  clk.min = min % 60;
}

void clock_inc_min(void)
{
  clk.min = (++ clk.min) % 60;
}

uint8_t clock_get_hour(void)
{
  return clk.hour;
}

void clock_set_hour(uint8_t hour)
{
  clk.hour = hour % 24;
}

void clock_inc_hour(void)
{
  clk.hour = (++ clk.hour) % 24;
}


uint8_t clock_get_date(void)
{
  return clk.date + 1;
}


void clock_set_date(uint8_t date)
{
  if(date == 0)
    return;
  clk.date = (date - 1) % clock_get_mon_date(clk.year, clk.mon);
  clock_recal_date_day();
}

void clock_inc_date(void)
{
  clk.date = ( ++ clk.date) % clock_get_mon_date(clk.year, clk.mon);
  clock_recal_date_day();
}

uint8_t clock_get_day(void)
{
  return clk.day + 1;
}

uint8_t clock_get_month(void)
{
  return clk.mon + 1;
}


void clock_set_month(uint8_t mon)
{
  if(mon == 0)
    return;
  clk.mon = (mon - 1) % 12;
  clock_recal_date_day();
}

void clock_inc_month(void)
{
  clk.mon = (++ clk.mon) % 12;
  clock_recal_date_day();
}

uint16_t clock_get_year(void)
{
  return clk.year;
}

void clock_set_year(uint16_t year)
{
  if(year >= 1901 && year <= 2099) {
    clk.year = year;
    clock_recal_date_day();
  }
}

void clock_inc_year(void)
{
  clk.year = (++ clk.year);
  if(clk.year > 2099)
    clk.year = 1901;
  clock_recal_date_day();
}

void clock_sync_from_rtc(enum clock_sync_type type)
{
  uint8_t year;
  bool centry;
  IVDBG("clock_sync_from_rtc = %u", type);
  if(type == CLOCK_SYNC_TIME) {
    rtc_get_time(&clk.hour, &clk.min, &clk.sec);
//    BSP_RTC_Read_Data(RTC_TYPE_TIME);
//    clk.hour = BSP_RTC_Time_Get_Hour();   // 0 - 23
//    clk.min  = BSP_RTC_Time_Get_Min();    // 0 - 59
//    clk.sec  = BSP_RTC_Time_Get_Sec();    // 0 - 59
    clk.ms39 = 255;   // 0 - 255
  } else if(type == CLOCK_SYNC_DATE) {
//    BSP_RTC_Read_Data(RTC_TYPE_DATE);
//    clk.year = BSP_RTC_Date_Get_Year();          // 0 - 99 (2000 ~ 2099)
//    clk.mon  = BSP_RTC_Date_Get_Month() - 1;     // 0 - 11
//    clk.date = BSP_RTC_Date_Get_Date() - 1;      // 0 - 30(29/28/27)
//    clk.day  = BSP_RTC_Date_Get_Day() - 1;       // 0 - 6
    centry = rtc_get_date(&year, &clk.mon, &clk.date, &clk.day);
    clk.mon  --;
    clk.date --;
    clk.day  --;
    if(centry) {
      clk.year = 2000 + year;
    } else {
      clk.year = 1900 + year;  
    }
  }
}

void clock_sync_to_rtc(enum clock_sync_type type)
{
  uint8_t year;
  bool centry = FALSE;  
  IVDBG("clock_sync_to_rtc = %u", type);
  if(type == CLOCK_SYNC_TIME) {
//    BSP_RTC_Read_Data(RTC_TYPE_TIME);
//    BSP_RTC_Time_Set_Hour(clk.hour);
//    BSP_RTC_Time_Set_Min(clk.min);
//    BSP_RTC_Time_Set_Sec(clk.sec);
//    BSP_RTC_Write_Data(RTC_TYPE_TIME);
    rtc_set_time(clk.hour, clk.min, clk.sec);
  } else if(type == CLOCK_SYNC_DATE) {
//    BSP_RTC_Read_Data(RTC_TYPE_DATE);
//    BSP_RTC_Date_Set_Year(clk.year);             // 0 - 99 (2000 ~ 2099)
//    BSP_RTC_Date_Set_Month(clk.mon + 1);         // 0 - 11
//    BSP_RTC_Date_Set_Date(clk.date + 1);         // 0 - 30(29/28/27)
//    BSP_RTC_Date_Set_Day(clk.day + 1);
//    BSP_RTC_Write_Data(RTC_TYPE_DATE);
    if(clk.year >= 2000) {
      centry = TRUE;
    }
    year = clk.year % 100;
    rtc_set_date(centry, year, clk.mon + 1, clk.date + 1, clk.day + 1);
  }
}

//static void clock0_ISR (void) interrupt 1 using 1
//{  
//
//  refresh_led();  
//  giff ++;
//  
//  if(in_console) {
//    TF0 = 0;
//    return;
//  }
//  if((giff % 2) && clock_tick_enabled) {
//    clock_inc_ms39();
//    timer_inc_ms39();
//  }
//  TF0 = 0;
//}

void clock_enable_interrupt(bool enable)
{
  rtc_enable_32768HZ(enable);
}

// 辅助函数
bool clock_is_leap_year(uint16_t year)
{
  return is_leap_year(year);
}

// 返回某一年某一月有几天
uint8_t clock_get_mon_date(uint16_t year, uint8_t mon)
{
  if(mon >= 12) mon = 11;
  if(is_leap_year(year))
    return date_table[0][mon];
  else
    return date_table[1][mon];
}


void clock_init(void)
{
  IVDBG(("clock_initialize"));
  
  if(button_is_factory_reset()) { //12:10:30 PM
    IVINFO("clock factory reset time");
//    BSP_RTC_Read_Data(RTC_TYPE_TIME);
//    BSP_RTC_Time_Set_Hour(12);
//    BSP_RTC_Time_Set_Min(10);
//    BSP_RTC_Time_Set_Sec(30); 
//    BSP_RTC_Write_Data(RTC_TYPE_TIME);
    rtc_set_time(
      CLOCK_FACTORY_RESET_HOUR,
      CLOCK_FACTORY_RESET_MIN,
      CLOCK_FACTORY_RESET_SEC);

    IVINFO("clock factory reset date");
//    BSP_RTC_Read_Data(RTC_TYPE_DATE);
//    BSP_RTC_Date_Set_Year(14);
//    BSP_RTC_Date_Set_Month(8);
//    BSP_RTC_Date_Set_Date(19);
    rtc_set_date(
      CLOCK_FACTORY_RESET_CENTRY,
      CLOCK_FACTORY_RESET_YEAR,
      CLOCK_FACTORY_RESET_MON,
      CLOCK_FACTORY_RESET_DATA,
      CLOCK_FACTORY_RESET_DAY);
  
//    BSP_RTC_Date_Set_Day(cext_yymmdd_to_day(
//    BSP_RTC_Date_Get_Year() ,
//    BSP_RTC_Date_Get_Month() - 1,
//    BSP_RTC_Date_Get_Date() - 1) + 1);
//  
//    BSP_RTC_Write_Data(RTC_TYPE_DATE);  
  }
  
  BSP_TIM2_Start();
  clock_sync_from_rtc(CLOCK_SYNC_TIME);
  clock_sync_from_rtc(CLOCK_SYNC_DATE); 
  clock_enable_interrupt(TRUE);
  refresh_display = FALSE;
  clock_is_hour12 = config_read_int("time_12");
  clock_dump();
}


void clock_enter_powersave(void)
{
  IVDBG(("clock_enter_powersave"));
  clock_enable_interrupt(FALSE);
  BSP_TIM2_Stop();
  
  // 保存时间，当醒来的时候看看有没有从20xx->1900
  // 是的话，得跨越到1901，因为rtc把1900当闰年
  // 同时如果穿越回去的话，得重新计算day保证形式上逻辑正确
  memcpy(&saved_clk, &clk, sizeof(saved_clk));
}

void clock_leave_powersave(void)
{
  IVDBG(("clock_leave_powersave"));
  
  clock_sync_from_rtc(CLOCK_SYNC_TIME);
  clock_sync_from_rtc(CLOCK_SYNC_DATE);
  
  if(saved_clk.year >=2000 && clk.year <= 1999) {
    if(clk.year == 1900) {
      clk.year = 1901;
    }
    clock_recal_date_day();
    clock_sync_to_rtc(CLOCK_SYNC_DATE); 
    alarm_resync_rtc();    
  }    
  
  BSP_TIM2_Start();
  clock_enable_interrupt(TRUE);
}

void clock_enter_console(void)
{
  clock_enable_interrupt(FALSE);
}

void clock_leave_console(void)
{
  clock_sync_from_rtc(CLOCK_SYNC_TIME);
  clock_sync_from_rtc(CLOCK_SYNC_DATE);
  clock_enable_interrupt(TRUE);
}

void clock_time_proc(enum task_events ev)
{
  if(ev == EV_1S) {
    display_mon_light();
  }
  sm_run(ev);
}
