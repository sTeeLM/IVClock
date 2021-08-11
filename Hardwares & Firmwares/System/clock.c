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


#define CLOCK_FACTORY_RESET_HOUR 12
#define CLOCK_FACTORY_RESET_MIN  11
#define CLOCK_FACTORY_RESET_SEC  0
#define CLOCK_FACTORY_RESET_YEAR 14
#define CLOCK_FACTORY_RESET_MON  8
#define CLOCK_FACTORY_RESET_DATA 19
#define CLOCK_FACTORY_RESET_DAY  2
      

static uint8_t date_table[2][12] = 
{
{31,29,31,30,31,30,31,31,30,31,30,31,}, // 2000
{31,28,31,30,31,30,31,31,30,31,30,31,},
};

static struct clock_struct clk;
static uint32_t now_sec; // 用于 time_diff

static bool refresh_display;

static bool in_console;
static bool clock_tick_enabled;

void clock_refresh_display_enable(bool enable)
{
  refresh_display = enable;
}


// 1 / 256 = 0.00390625
void clock_inc_ms39(void)
{
  int16_t y;
  clk.ms39 ++;
  
  if(in_console)
    return;
  
  if(clk.ms39 == 0 ) {
    clk.sec = (++ clk.sec) % 60;
    now_sec ++;

    if(clk.sec == 0) {
      clk.min = (++ clk.min) % 60;
      if(clk.min == 0) {
        clk.hour = (++ clk.hour) % 24;
        if(clk.hour == 0) {
          y = clk.year + CEXT_YEAR_BASE;
          if(is_leap_year(y)) {
            clk.date = (++ clk.date) % date_table[0][clk.mon];
          } else {
            clk.date = (++ clk.date) % date_table[1][clk.mon];
          }
          clk.day = (++ clk.day) % 7;
          if(clk.day == 0) {
            clk.mon = (++ clk.mon) % 12;
            if(clk.mon == 0) {
              clk.year = (++ clk.year) % 100;
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
  console_printf("%02d-%02d-%02d %02d:%02d:%02d:%03d\r\n",
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
void clock_clr_sec(void)
{
   clk.sec = 0;
}

uint8_t clock_get_min(void)
{
  return clk.min;
}
void clock_inc_min(void)
{
  clk.min = (++ clk.min) % 60;
}

uint8_t clock_get_hour(void)
{
  return clk.hour;
}
void clock_inc_hour(void)
{
  clk.hour = (++ clk.hour) % 24;
}


uint8_t clock_get_date(void)
{
  return clk.date + 1;
}
void clock_inc_date(void)
{
  clk.date = ( ++ clk.date) % clock_get_mon_date(clk.year, clk.mon);
  clk.day = cext_yymmdd_to_day(clk.year, clk.mon, clk.date);
}

uint8_t clock_get_day(void)
{
  return clk.day + 1;
}

uint8_t clock_get_month(void)
{
  return clk.mon + 1;
}
void clock_inc_month(void)
{
  clk.mon = (++ clk.mon) % 12;
  // 如果是2月并且不是闰年，需要保证不能出现2月29日
  // 以及不能出现4月31日这样的情况
  if(clk.date > clock_get_mon_date(clk.year, clk.mon) - 1)
    clk.date = clock_get_mon_date(clk.year, clk.mon) - 1;
  clk.day = cext_yymmdd_to_day(clk.year, clk.mon, clk.date);
}

uint8_t clock_get_year(void)
{
  return clk.year;
}
void clock_inc_year(void)
{
  clk.year = (++ clk.year) % 100;
  // 如果是2月并且不是闰年，需要保证不能出现2月29日
  // 以及不能出现4月31日这样的情况
  if(clk.date > clock_get_mon_date(clk.year, clk.mon) - 1)
    clk.date = clock_get_mon_date(clk.year, clk.mon) - 1;
  clk.day = cext_yymmdd_to_day(clk.year, clk.mon, clk.date);
}

void clock_sync_from_rtc(enum clock_sync_type type)
{
  IVDBG("clock_sync_from_rtc = %u", type);
  clock_enable_interrupt(FALSE);
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
    rtc_get_date(&clk.year, &clk.mon, &clk.date, &clk.day);
    clk.mon  --;
    clk.date --;
    clk.day  --;
  }
  clock_enable_interrupt(TRUE);
}

void clock_sync_to_rtc(enum clock_sync_type type)
{
  IVDBG("clock_sync_to_rtc = %u", type);
  clock_enable_interrupt(FALSE);
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
    rtc_set_date(clk.year, clk.mon + 1, clk.date + 1, clk.day + 1);
  }
  clock_enable_interrupt(TRUE);
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
bool clock_is_leap_year(uint8_t year)
{
  uint16_t y;
  if(year >= 100) year = 99;
  y = year + CEXT_YEAR_BASE;
  return is_leap_year(y);
}

// 返回某一年某一月有几天
uint8_t clock_get_mon_date(uint8_t year, uint8_t mon)
{
  if(year >= 100) year = 99;
  if(mon >= 12) mon = 11;
  if(clock_is_leap_year(year))
    return date_table[0][mon];
  else
    return date_table[1][mon];
}


void clock_init(void)
{
  IVDBG(("clock_initialize"));
  
  in_console = FALSE;
  
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
  
  clock_sync_from_rtc(CLOCK_SYNC_TIME);
  clock_sync_from_rtc(CLOCK_SYNC_DATE); 
  refresh_display = FALSE;
  clock_dump();
}


void clock_enter_powersave(void)
{
  IVDBG(("clock_enter_powersave"));
  clock_enable_interrupt(FALSE);
}

void clock_leave_powersave(void)
{
  IVDBG(("clock_leave_powersave"));
  clock_sync_from_rtc(CLOCK_SYNC_TIME);
  clock_sync_from_rtc(CLOCK_SYNC_DATE);
  clock_enable_interrupt(TRUE);
}

void clock_enter_console(void)
{
  in_console = TRUE;
}

void clock_leave_console(void)
{
  in_console = FALSE;
  clock_enable_interrupt(FALSE);
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
