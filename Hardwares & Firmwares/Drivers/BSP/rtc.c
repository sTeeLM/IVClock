#include "rtc.h"
#include "bsp.h"
#include "debug.h"
#include "i2c.h"
#include "key.h"
#include "delay.h"
#include "cext.h"

#include <string.h>

#define RTC_I2C_ADDRESS  0xD0 //11010000

#define RTC_TIME_OFFSET 0x00
#define RTC_DATE_OFFSET 0x03
#define RTC_ALARM0_OFFSET 0x07
#define RTC_ALARM1_OFFSET 0x0B
#define RTC_CTL_OFFSET 0x0E
#define RTC_TEMP_OFFSET 0x11

#define RTC_CTL_OFFSET 0x0E

static uint8_t _rtc_data[4];
static uint8_t _last_read;
static uint8_t _is_lt_timer_mode;

static const char * rtc_alarm_mode_str[] = 
{
  "RTC_ALARM0_MOD_PER_SEC",
  "RTC_ALARM0_MOD_MATCH_SEC",  
  "RTC_ALARM0_MOD_MATCH_MIN_SEC", 
  "RTC_ALARM0_MOD_MATCH_HOUR_MIN_SEC", 
  "RTC_ALARM0_MOD_MATCH_DATE_HOUR_MIN_SEC",   
  "RTC_ALARM0_MOD_MATCH_DAY_HOUR_MIN_SEC",
  "RTC_ALARM0_MOD_CNT", 
  "RTC_ALARM1_MOD_PER_MIN",
  "RTC_ALARM1_MOD_MATCH_MIN",  
  "RTC_ALARM1_MOD_MATCH_HOUR_MIN", 
  "RTC_ALARM1_MOD_MATCH_DATE_HOUR_MIN",   
  "RTC_ALARM1_MOD_MATCH_DAY_HOUR_MIN", 
};

static const char * rtc_square_rate_str[] = 
{
  "RTC_SQUARE_RATE_1HZ",
  "RTC_SQUARE_RATE_1024HZ",
  "RTC_SQUARE_RATE_4096HZ",
  "RTC_SQUARE_RATE_8192HZ"
};

static void _rtc_dump_raw(void)
{
  uint8_t addr;
  uint8_t c;
	BSP_Error_Type ret;
  IVDBG("RTC raw content:");
  for(addr = 0; addr < 0x12; addr ++) {
    BSP_I2C_Read(RTC_I2C_ADDRESS, addr, I2C_MEMADD_SIZE_8BIT, &c, 1);
    IVDBG("rtc [%02x] = 0x%02x", addr, c);
  }
}

static void _rtc_initialize (void)
{
  uint8_t count;

  IVDBG("RTC before initialize:");
  _rtc_dump_raw();
  BSP_RTC_Dump();
  
  _is_lt_timer_mode = 0;
    
  memset(_rtc_data, 0, sizeof(_rtc_data));

  // 初始化

  BSP_RTC_Read_Data(RTC_TYPE_TIME);

  // RTC 内部使用24小时制
  count = BSP_RTC_Time_Get_Hour();
  BSP_RTC_Time_Set_Hour_12(0);
  BSP_RTC_Time_Set_Hour(count);
  
  ///// 调试用，2014-08-19, 12:10:30 PM
  // BSP_RTC_Time_Set_Hour(12);
  // BSP_RTC_Time_Set_Min(10);
  // BSP_RTC_Time_Set_Sec(30); 
  /////
  
  if(BSP_Key_Is_Factory_Reset()) { //12:10:30 PM
		IVDBG("RTC factory reset time");
    BSP_RTC_Time_Set_Hour(12);
    BSP_RTC_Time_Set_Min(10);
    BSP_RTC_Time_Set_Sec(30); 
  }
  BSP_RTC_Write_Data(RTC_TYPE_TIME);
	
  IVDBG("before delay");
  delay_ms(10);
	IVDBG("after delay");
  
  BSP_RTC_Read_Data(RTC_TYPE_DATE);
 
  ///// 调试用，初始时钟设置为 12小时格式，2014-08-19, 12:10：30 AM
  // BSP_RTC_Date_Set_Year(14);
  // BSP_RTC_Date_Set_Month(8);
  // BSP_RTC_Date_Set_Date(19);
  /////
  
  if(BSP_Key_Is_Factory_Reset()) { // 2014-08-19
		IVDBG("RTC factory reset date");
    BSP_RTC_Date_Set_Year(14);
    BSP_RTC_Date_Set_Month(8);
    BSP_RTC_Date_Set_Date(19);
  }
  
  BSP_RTC_Date_Set_Day(cext_yymmdd_to_day(
    BSP_RTC_Date_Get_Year() ,
    BSP_RTC_Date_Get_Month() - 1,
    BSP_RTC_Date_Get_Date() - 1) + 1);
  
  BSP_RTC_Write_Data(RTC_TYPE_DATE);
  
  delay_ms(10);
  
  // 闹钟也设置为24小时格式
  BSP_RTC_Read_Data(RTC_TYPE_ALARM0);
  BSP_RTC_Alarm_Set_Hour_12(0);
  BSP_RTC_Write_Data(RTC_TYPE_ALARM0);  
  
  BSP_RTC_Read_Data(RTC_TYPE_ALARM1);
  BSP_RTC_Alarm_Set_Hour_12(0);
  BSP_RTC_Write_Data(RTC_TYPE_ALARM1);

  
  // 清除所有闹钟：闹钟配置由alarm自行从rom中读取，写入rtc
  BSP_RTC_Read_Data(RTC_TYPE_CTL);
  BSP_RTC_Enable_Alarm_Int(RTC_ALARM0, 0);
  BSP_RTC_Clr_Alarm_Int_Flag(RTC_ALARM0);
  BSP_RTC_Enable_Alarm_Int(RTC_ALARM1, 0);
  BSP_RTC_Clr_Alarm_Int_Flag(RTC_ALARM1);
  // 允许RTC发中断
  BSP_RTC_Set_Intcn(1);
  // 启动32KHZ输出  
  BSP_RTC_Set_En32khz(1);
  // 清除eosc
  BSP_RTC_Set_Eosc(0);
  // BBSQW = 0，电池供电时候没有方波或者中断
  BSP_RTC_Set_Bbsqw(0);
  BSP_RTC_Write_Data(RTC_TYPE_CTL); 
  
  IVDBG("RTC after initialize:");
  _rtc_dump_raw();
  BSP_RTC_Dump();
}

BSP_Error_Type BSP_RTC_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : INT_ALARM_Pin */
  GPIO_InitStruct.Pin = INT_ALARM_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(INT_ALARM_GPIO_Port, &GPIO_InitStruct);
	
  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(INT_ALARM_EXTI_IRQn, BSP_ALARM_IRQ_PRIORITY, BSP_ALARM_IRQ_SUB_PRIORITY);
  HAL_NVIC_EnableIRQ(INT_ALARM_EXTI_IRQn);
	
  _rtc_initialize();

	return BSP_ERROR_NONE;
}


void BSP_RTC_Dump(void)
{
  IVDBG("BSP_RTC_Dump:");
  BSP_RTC_Read_Data(RTC_TYPE_DATE);
  IVDBG("date/day: %02u-%02u-%02u/%u",
    BSP_RTC_Date_Get_Year(), BSP_RTC_Date_Get_Month(), BSP_RTC_Date_Get_Date(),
    BSP_RTC_Date_Get_Day()
  );
  
  BSP_RTC_Read_Data(RTC_TYPE_TIME);
  IVDBG("time: %02u:%02u:%02u, is12: %s",
    BSP_RTC_Time_Get_Hour(), BSP_RTC_Time_Get_Min(), BSP_RTC_Time_Get_Sec(),
    BSP_RTC_Time_Get_Hour_12() ? "ON" : "OFF"
  );
  
  BSP_RTC_Read_Data(RTC_TYPE_ALARM0);
  IVDBG("alarm0 mode: %s", BSP_RTC_Alarm_Get_Mode_Str());
  IVDBG("  day:%02u", BSP_RTC_Alarm_Get_Day());
  IVDBG("  date:%02u", BSP_RTC_Alarm_Get_Date());  
  IVDBG("  hour:%02u", BSP_RTC_Alarm_Get_Hour());
  IVDBG("  min:%02u", BSP_RTC_Alarm_Get_Min());  
  IVDBG("  sec:%02u", BSP_RTC_Alarm_Get_Sec());
  IVDBG("  is12:%s", BSP_RTC_Alarm_Get_Hour_12() ? "ON" : "OFF");  

  BSP_RTC_Read_Data(RTC_TYPE_ALARM1);
  IVDBG("alarm1 mode: %s", BSP_RTC_Alarm_Get_Mode_Str());
  IVDBG("  day:%02u", BSP_RTC_Alarm_Get_Day());
  IVDBG("  date:%02u", BSP_RTC_Alarm_Get_Date());  
  IVDBG("  hour:%02u", BSP_RTC_Alarm_Get_Hour());
  IVDBG("  min:%02u", BSP_RTC_Alarm_Get_Min());  
  IVDBG("  is12:%s", BSP_RTC_Alarm_Get_Hour_12() ? "ON" : "OFF");
  
  BSP_RTC_Read_Data(RTC_TYPE_CTL);
  IVDBG("control:");
  IVDBG("  alarm0 int enable:%s", BSP_RTC_Test_Alarm_Int(RTC_ALARM0) ? "ON" : "OFF");
  IVDBG("  alarm1 int enable:%s", BSP_RTC_Test_Alarm_Int(RTC_ALARM1) ? "ON" : "OFF");
  IVDBG("  alarm0 int flag:%s", BSP_RTC_Test_Alarm_Int_Flag(RTC_ALARM0) ? "ON" : "OFF");
  IVDBG("  alarm1 int flag:%s", BSP_RTC_Test_Alarm_Int_Flag(RTC_ALARM1) ? "ON" : "OFF");
  IVDBG("  eosc:%c", BSP_RTC_Test_Eosc() ? '1' : '0');  
  IVDBG("  bbsqw:%c", BSP_RTC_Test_Bbsqw() ? '1' : '0');  
  IVDBG("  conv:%c", BSP_RTC_Test_Conv() ? '1' : '0');  
  IVDBG("  square_rate:%s", BSP_RTC_Get_Square_Rate_Str());
  IVDBG("  intcn:%c", BSP_RTC_Test_Intcn() ? '1' : '0');  
  IVDBG("  osf:%c", BSP_RTC_Test_Osf() ? '1' : '0');  
  IVDBG("  en32khz:%c", BSP_RTC_Test_En32khz() ? '1' : '0');  
  IVDBG("  bsy:%c", BSP_RTC_Test_Bsy() ? '1' : '0');  
}

bool BSP_RTC_Is_Lt_Timer(void)
{
  return _is_lt_timer_mode;
}

void BSP_RTC_Set_Lt_Timer(bool enable)
{
  _is_lt_timer_mode = enable;
}

void BSP_RTC_Read_Data(enum BSP_RTC_Data_Type type)
{
  uint8_t offset;
  switch(type) {
    case RTC_TYPE_TIME:
      offset = RTC_TIME_OFFSET; break;
    case RTC_TYPE_DATE:
      offset = RTC_DATE_OFFSET; break;    
    case RTC_TYPE_ALARM0:
      offset = RTC_ALARM0_OFFSET; break;
    case RTC_TYPE_ALARM1:
      offset = RTC_ALARM1_OFFSET; break;
    case RTC_TYPE_TEMP:
      offset = RTC_TEMP_OFFSET; break; 
    case RTC_TYPE_CTL:
      offset = RTC_CTL_OFFSET; break;     
  }
  
  _last_read = type;
  
	BSP_I2C_Read(RTC_I2C_ADDRESS, offset, I2C_MEMADD_SIZE_8BIT, _rtc_data, sizeof(_rtc_data));
  
}

void BSP_RTC_Write_Data(enum BSP_RTC_Data_Type type)
{
  uint8_t offset;
  switch(type) {
    case RTC_TYPE_TIME:
      offset = RTC_TIME_OFFSET; break;
    case RTC_TYPE_DATE:
      offset = RTC_DATE_OFFSET; break;    
    case RTC_TYPE_ALARM0:
      offset = RTC_ALARM0_OFFSET; break;
    case RTC_TYPE_ALARM1:
      offset = RTC_ALARM1_OFFSET; break;
    case RTC_TYPE_TEMP:
      offset = RTC_TEMP_OFFSET; break; 
    case RTC_TYPE_CTL:
      offset = RTC_CTL_OFFSET; break;       
  }
	
	BSP_I2C_Write(RTC_I2C_ADDRESS, offset, I2C_MEMADD_SIZE_8BIT, _rtc_data, sizeof(_rtc_data));
   
}

static uint8_t _rtc_get_hour(uint8_t hour)
{
  uint8_t ret;
  if(hour & 0x40) { // 是12小时表示
    ret = (hour & 0x0F) + ((hour & 0x10) >> 4) * 10;
    if( hour & 0x20 ) { // 是PM
      ret += 12;
    }
  } else { // 是24小时表示
      ret =  (hour & 0x0F) + ((hour & 0x30) >> 4) * 10;
  }
  return ret;
}


static void _rtc_set_hour(uint8_t hour, uint8_t * dat)
{
  if(*dat & 0x40) { // 是12小时表示
    if(hour > 12) {
      *dat |= 0x20; // 设置PM标志
      hour -= 12;
    } else {
      *dat &= ~0x20; // 清除PM标志
    }
    *dat &= 0xF0; // clear lsb
    *dat |= hour % 10;
    *dat &= 0xEF; // clear msb
    *dat |= (hour / 10) << 4;    
  } else {
    // 24小时表示
    *dat &= 0xF0; // clear lsb
    *dat |= hour % 10;
    *dat &= 0xCF; // clear msb
    *dat |= (hour / 10) << 4;
  }  
}

static void _rtc_set_hour_12(bool enable, uint8_t * dat)
{
  uint8_t hour;
  hour = _rtc_get_hour(* dat);
  if(enable) {
    *dat |= 0x40;
  } else {
    *dat &= ~0x40;
  }
  _rtc_set_hour(hour, dat);  
}

static bool _rtc_get_hour_12(uint8_t hour)
{
  return ((hour & 0x40) != 0);
}

static uint8_t _rtc_get_min_sec(uint8_t min)
{
  return (min & 0x0F) + ((min & 0x70) >> 4) * 10;
}

static void _rtc_set_min_sec(uint8_t min, uint8_t * dat)
{
  *dat &= 0xF0;
  *dat |= min % 10;
  *dat &= 0x8F;
  *dat |= (min / 10) << 4;
}

// 在read__rtc_data(RTC_TYPE_TIME)之后调用
// 此函数始终返回24小时格式的时间！
uint8_t BSP_RTC_Time_Get_Hour(void)
{
  return _rtc_get_hour(_rtc_data[2]);
}


// 此函数时钟传入24小时格式的时间
void BSP_RTC_Time_Set_Hour(uint8_t hour)
{
  _rtc_set_hour(hour, &_rtc_data[2]);
}

void BSP_RTC_Time_Set_Hour_12(bool enable)
{
  _rtc_set_hour_12(enable, &_rtc_data[2]);
}

bool BSP_RTC_Time_Get_Hour_12()
{
  return _rtc_get_hour_12(_rtc_data[2]);
}

uint8_t BSP_RTC_Time_Get_Min(void)
{
  return _rtc_get_min_sec(_rtc_data[1]);
}

void BSP_RTC_Time_Set_Min(uint8_t min)
{
  _rtc_set_min_sec(min, &_rtc_data[1]);
}

uint8_t BSP_RTC_Time_Get_Sec(void)
{
  return _rtc_get_min_sec(_rtc_data[0]);
}

void BSP_RTC_Time_Set_Sec(uint8_t sec)
{
  _rtc_set_min_sec(sec, &_rtc_data[0]);
}

// 在rtc_read_data(RTC_TYPE_DATE)之后调用
uint8_t BSP_RTC_Date_Get_Year()
{
  return (_rtc_data[3] & 0x0F) + ((_rtc_data[3] & 0xF0) >> 4) * 10;
}

void BSP_RTC_Date_Set_Year(uint8_t year)
{
  _rtc_data[3] &= 0xF0;
  _rtc_data[3] |= year % 10;
  _rtc_data[3] &= 0x0F;
  _rtc_data[3] |= (year / 10) << 4;  
}

uint8_t BSP_RTC_Date_Get_Month()
{
  return (_rtc_data[2] & 0x0F) + ((_rtc_data[2] & 0x10) >> 4) * 10;
}

void BSP_RTC_Date_Set_Month(uint8_t month)
{
  _rtc_data[2] &= 0xF0;
  _rtc_data[2] |= month % 10;
  _rtc_data[2] &= 0x0F;
  _rtc_data[2] |= (month / 10) << 4;  
}

static uint8_t _rtc_get_date(uint8_t date)
{
  return (date & 0x0F) + ((date & 0x30) >> 4) * 10;
}

static void _rtc_set_date(uint8_t date, uint8_t * dat)
{
  *dat &= 0xF0;
  *dat |= date % 10;
  *dat &= 0xCF;
  *dat |= (date / 10) << 4; 
}

uint8_t BSP_RTC_Date_Get_Date()
{
  return _rtc_get_date(_rtc_data[1]);
}

// 此函数需要检查合法性！！
bool BSP_RTC_Date_Set_Date(uint8_t date)
{
  uint8_t mon = BSP_RTC_Date_Get_Month();
  
  IVDBG("rtc_date_set_day, valid check...");
  if(mon == 1 || mon == 3 || mon == 5 || mon == 7 
    || mon == 8 || mon == 10 || mon == 12) {
    if(date > 32) return 1;
  } else {
    if(date > 31) return 1;
    if(mon == 2 && cext_is_leap_year(BSP_RTC_Date_Get_Year())) {
      if(date > 30) return 1;
    } else if(mon == 2 && !cext_is_leap_year(BSP_RTC_Date_Get_Year())) {
      if(date > 29) return 1;
    }
  }
  
  IVDBG("rtc_date_set_day, valid check...OK");
  _rtc_set_date(date, &_rtc_data[1]);

  return 0;  
}

uint8_t BSP_RTC_Date_Get_Day()
{
  return _rtc_data[0];
}

void BSP_RTC_Date_Set_Day(uint8_t day)
{
  if (day >= 1 && day <= 7) {
    _rtc_data[0] = day;
  }
}

// 在rtc_read_data(RTC_TYPE_ALARM0)或者RTC_TYPE_ALARM1之后调用
uint8_t BSP_RTC_Alarm_Get_Hour()
{
  return _rtc_get_hour(_last_read == RTC_TYPE_ALARM0 ? _rtc_data[2]:_rtc_data[1]);
}

bool BSP_RTC_Alarm_Get_Hour_12()
{
  return _rtc_get_hour_12(_last_read == RTC_TYPE_ALARM0 ? _rtc_data[2] : _rtc_data[1]);
}

void BSP_RTC_Alarm_Set_Hour_12(bool enable)
{
  _rtc_set_hour_12(enable, _last_read == RTC_TYPE_ALARM0 ? &_rtc_data[2] : &_rtc_data[1]);
}

void BSP_RTC_Alarm_Set_Hour(uint8_t hour)
{
  _rtc_set_hour(hour, _last_read == RTC_TYPE_ALARM0 ? &_rtc_data[2] : &_rtc_data[1]);
}

uint8_t BSP_RTC_Alarm_Get_Min()
{
  return _rtc_get_min_sec(_last_read == RTC_TYPE_ALARM0 ? _rtc_data[1] : _rtc_data[0]);
}

void BSP_RTC_Alarm_Set_Min( uint8_t min)
{
  _rtc_set_min_sec(min, _last_read == RTC_TYPE_ALARM0 ? &_rtc_data[1] : &_rtc_data[0]); 
}

uint8_t BSP_RTC_Alarm_Get_Day(void)
{
  return _last_read == RTC_TYPE_ALARM0 ? (_rtc_data[3] & 0x0F) : (_rtc_data[2] & 0x0F);
}

void BSP_RTC_Alarm_Set_Day(uint8_t day)
{
  if(_last_read == RTC_TYPE_ALARM0) {
    _rtc_data[3] &= 0xF0;
    _rtc_data[3] |= day;
  } else {
    _rtc_data[2] &= 0xF0;
    _rtc_data[2] |= day;
  }
}

uint8_t BSP_RTC_Alarm_Get_Date(void)
{
  return _last_read == RTC_TYPE_ALARM0 ?
    _rtc_get_date(_rtc_data[3]) : _rtc_get_date(_rtc_data[2]);
}

void BSP_RTC_Alarm_Set_Date(uint8_t date)
{
  _last_read == RTC_TYPE_ALARM0 ? _rtc_set_date(date, &_rtc_data[3]) : 
    _rtc_set_date(date, &_rtc_data[2]);
}

uint8_t BSP_RTC_Alarm_Get_Sec(void)
{
  if(_last_read == RTC_TYPE_ALARM0) {
    return (_rtc_data[0] & 0x0F) + ((_rtc_data[0] & 0x70) >> 4) * 10;
  }
  return 0;
}


void BSP_RTC_Alarm_Set_Sec( uint8_t sec)
{
  if(_last_read == RTC_TYPE_ALARM0) {
    _rtc_data[0] &= 0xF0;
    _rtc_data[0] |= sec % 10;
    _rtc_data[0] &= 0x8F;
    _rtc_data[0] |= ((sec / 10) << 4);    
  }
}
// DY A1M4 A1M3 A1M2 A1M1
// X  1    1    1    1    ALARM0_MOD_PER_SEC                 Alarm once per second
// X  1    1    1    0    ALARM0_MOD_MATCH_SEC               Alarm when seconds match
// X  1    1    0    0    ALARM0_MOD_MATCH_MIN_SEC           Alarm when minutes and seconds match
// X  1    0    0    0    ALARM0_MOD_MATCH_HOUR_MIN_SEC      Alarm when hours, minutes, and seconds match
// 0  0    0    0    0    ALARM0_MOD_MATCH_DATE_HOUR_MIN_SEC Alarm when date, hours, minutes, and seconds match
// 1  0    0    0    0    ALARM0_MOD_MATCH_DAY_HOUR_MIN_SEC  Alarm when day, hours, minutes, and seconds match

// DY A2M4 A2M3 A1M2
// X  1    1    1    ALARM1_MOD_PER_MIN                 Alarm once per minute (00 seconds of every minute)
// X  1    1    0    ALARM1_MOD_MATCH_MIN               Alarm when minutes match
// X  1    0    0    ALARM1_MOD_MATCH_HOUR_MIN          Alarm when hours and minutes match
// 0  0    0    0    ALARM1_MOD_MATCH_DATE_HOUR_MIN     Alarm when date, hours, and minutes match
// 1  0    0    0    ALARM1_MOD_MATCH_DAY_HOUR_MIN      Alarm when day, hours, and minutes match

enum BSP_RTC_Alarm_Mode BSP_RTC_Alarm_Get_Mode(void)
{
  uint8_t dy, a1m1, a1m2, a1m3, a1m4;
  if(_last_read == RTC_TYPE_ALARM0) {
    dy = _rtc_data[3] & 0x40;
    a1m1 = _rtc_data[0] & 0x80;
    a1m2 = _rtc_data[1] & 0x80;
    a1m3 = _rtc_data[2] & 0x80;
    a1m4 = _rtc_data[3] & 0x80;
    if(a1m1 && a1m2 && a1m3 && a1m4) {
      return RTC_ALARM0_MOD_PER_SEC;
    } else if(!a1m1 && a1m2 && a1m3 && a1m4) {
      return RTC_ALARM0_MOD_MATCH_SEC;
    } else if(!a1m1 && !a1m2 && a1m3  && a1m4) {
      return RTC_ALARM0_MOD_MATCH_MIN_SEC;
    } else if(!a1m1 && !a1m2 && !a1m3  && a1m4) {
      return RTC_ALARM0_MOD_MATCH_HOUR_MIN_SEC;
    } else if(!a1m1 && !a1m2 && !a1m3  && !a1m4 && !dy) {
      return RTC_ALARM0_MOD_MATCH_DATE_HOUR_MIN_SEC;
    } else {
      return RTC_ALARM0_MOD_MATCH_DAY_HOUR_MIN_SEC;
    }
  } else if(_last_read == RTC_TYPE_ALARM1){
    dy = _rtc_data[2] & 0x40;
    a1m2 = _rtc_data[0] & 0x80;
    a1m3 = _rtc_data[1] & 0x80;
    a1m4 = _rtc_data[2] & 0x80;
    if(a1m2 && a1m3 && a1m4) {
      return RTC_ALARM1_MOD_PER_MIN;
    } else if(!a1m2 && a1m3 && a1m4) {
      return RTC_ALARM1_MOD_MATCH_MIN;
    } else if(!a1m2 && !a1m3  && a1m4) {
      return RTC_ALARM1_MOD_MATCH_HOUR_MIN;
    } else if(!a1m2 && !a1m3  && !a1m4) {
      return RTC_ALARM1_MOD_MATCH_DATE_HOUR_MIN;
    } else if(!a1m2 && !a1m3  && !a1m4 && !dy) {
      return RTC_ALARM1_MOD_MATCH_DATE_HOUR_MIN;
    } else {
      return RTC_ALARM1_MOD_MATCH_DAY_HOUR_MIN;
    }
  }
  return RTC_ALARM0_MOD_CNT;
}
void BSP_RTC_Alarm_Set_Mod(enum BSP_RTC_Alarm_Mode mode)
{
  if(mode < RTC_ALARM0_MOD_CNT && _last_read == RTC_TYPE_ALARM0) {
    _rtc_data[0] &= ~0x80;
    _rtc_data[1] &= ~0x80;
    _rtc_data[2] &= ~0x80;
    _rtc_data[3] &= ~0x80; 
    _rtc_data[3] &= ~0x40;    
    switch(mode) {
      case RTC_ALARM0_MOD_PER_SEC:
        _rtc_data[0] |=  0x80;
      case RTC_ALARM0_MOD_MATCH_SEC:
        _rtc_data[1] |=  0x80;
      case RTC_ALARM0_MOD_MATCH_MIN_SEC:
        _rtc_data[2] |=  0x80;
      case RTC_ALARM0_MOD_MATCH_HOUR_MIN_SEC:
        _rtc_data[3] |=  0x80;
      case RTC_ALARM0_MOD_MATCH_DATE_HOUR_MIN_SEC:
        break;
      case RTC_ALARM0_MOD_MATCH_DAY_HOUR_MIN_SEC:
        _rtc_data[3] |= 0x40;
			default:
				;
    }
  } else if(mode > RTC_ALARM0_MOD_CNT && _last_read == RTC_TYPE_ALARM1) {
    _rtc_data[0] &= ~0x80;
    _rtc_data[1] &= ~0x80;
    _rtc_data[2] &= ~0x80;
    _rtc_data[2] &= ~0x40;
    switch (mode) {
      case RTC_ALARM1_MOD_PER_MIN:
        _rtc_data[0] |=  0x80;
      case RTC_ALARM1_MOD_MATCH_MIN:
        _rtc_data[1] |=  0x80;
      case RTC_ALARM1_MOD_MATCH_HOUR_MIN:
        _rtc_data[2] |=  0x80;
      case RTC_ALARM1_MOD_MATCH_DATE_HOUR_MIN:
        break;
      case RTC_ALARM1_MOD_MATCH_DAY_HOUR_MIN:
        _rtc_data[2] |= 0x40;
			default:
				;
    }
  }
}

const char * BSP_RTC_Alarm_Get_Mode_Str(void)
{
  return rtc_alarm_mode_str[BSP_RTC_Alarm_Get_Mode()];
}

// 在rtc_read_data(RTC_TYPE_TEMP)之后调用
bool BSP_RTC_Get_Temperature(uint8_t * integer, uint8_t * flt)
{
  float ret = 0;
  bool sign = ((_rtc_data[0] &  0x80) != 0);

  
  if(sign) { // 是负数
    _rtc_data[0] = ~_rtc_data[0];
    _rtc_data[1] &= 0xC0;
    _rtc_data[1] >>= 6;
    _rtc_data[1] = ~_rtc_data[1] + 1;
    _rtc_data[1] &= 0x03;
    if(_rtc_data[1] == 0) {
      _rtc_data[0] ++;
    }
  } else { //是正数
    _rtc_data[1] &= 0xC0;
    _rtc_data[1] >>= 6;
  }
  
  *integer = _rtc_data[0];
  
  if((_rtc_data[1] & 0x3) == 0x3) {
    *flt = 75;
  } else if((_rtc_data[1] & 0x3) == 0x2) { 
    *flt = 50;
  } else if((_rtc_data[1] & 0x3) == 0x1) { 
    *flt = 25;
  } else {
    *flt = 0;
  }

  if(*integer > 99) *integer = 99;
 
  return sign;
}

// 在rtc_read_data（RTC_TYPE_CTL）之后调用
void BSP_RTC_Enable_Alarm_Int(enum BSP_RTC_Alarm_Index index, bool enable)
{
  if(index == RTC_ALARM0) {
    if(!enable)
      _rtc_data[0] &= ~1;
    else
      _rtc_data[0] |= 1;
  } else if(index == RTC_ALARM1) {
    if(!enable)
      _rtc_data[0] &= ~2;
    else
      _rtc_data[0] |= 2;
  }
}

bool BSP_RTC_Test_Alarm_Int(enum BSP_RTC_Alarm_Index index)
{
  if(index == RTC_ALARM0) {
    return (_rtc_data[0] & 1) == 1;
  } else if(index == RTC_ALARM1) {
    return (_rtc_data[0] & 2) == 2;
  }
  return 0;
}

void BSP_RTC_Clr_Alarm_Int_Flag(enum BSP_RTC_Alarm_Index index)
{
  if(index == RTC_ALARM0) {
    _rtc_data[1] &= ~1;
  } else if(index == RTC_ALARM1) {
    _rtc_data[1] &= ~2;    
  } 
}

bool BSP_RTC_Test_Alarm_Int_Flag(enum BSP_RTC_Alarm_Index index)
{
  if(index == RTC_ALARM0) {
    return (_rtc_data[1] & 1) == 1;
  } else if(index == RTC_ALARM1) {
    return (_rtc_data[1] & 2) == 2;
  }
  return 0;
}

bool BSP_RTC_Test_Eosc(void)
{
  return (_rtc_data[0] & 0x80) != 0;
}

void BSP_RTC_Set_Eosc(bool val)
{
  _rtc_data[0] &= ~0x80;
  if(val)
    _rtc_data[0] |= 0x80;
}

bool BSP_RTC_Test_Bbsqw(void)
{
  return (_rtc_data[0] & 0x40) != 0;
}

void BSP_RTC_Set_Bbsqw(bool val)
{
  _rtc_data[0] &= ~0x40;
  if(val)
    _rtc_data[0] |= 0x40;
}

bool BSP_RTC_Test_Conv(void)
{
  return (_rtc_data[0] & 0x20) != 0;
}

void BSP_RTC_Set_conv(bool val)
{
  _rtc_data[0] &= ~0x20;
  if(val)
    _rtc_data[0] |= 0x20;
}

enum BSP_RTC_Square_Rate BSP_RTC_Get_Square_Rate(void)
{
  return (((_rtc_data[0] & 0x18) >> 3) & 0x3);
}

void BSP_RTC_Set_Square_Rate(enum BSP_RTC_Square_Rate rt)
{
  uint8_t val = rt;
  _rtc_data[0] &= ~0x18;
  _rtc_data[0] |= val << 3;
}

const char * BSP_RTC_Get_Square_Rate_Str(void)
{
  return rtc_square_rate_str[BSP_RTC_Get_Square_Rate()];
}

bool BSP_RTC_Test_Intcn(void)
{
  return (_rtc_data[0] & 0x4) != 0;
}

void BSP_RTC_Set_Intcn(bool val)
{
  _rtc_data[0] &= ~0x4;
  if(val)
    _rtc_data[0] |= 0x4;
}

bool BSP_RTC_Test_Osf(void)
{
  return (_rtc_data[1] & 0x80) != 0;
}

void BSP_RTC_Set_Osf(bool val)
{
  _rtc_data[1] &= ~0x80;
  if(val)
    _rtc_data[1] |= 0x80;
}

bool BSP_RTC_Test_En32khz(void)
{
  return (_rtc_data[1] & 0x8) != 0;
}

void BSP_RTC_Set_En32khz(bool val)
{
  _rtc_data[1] &= ~0x8;
  if(val)
    _rtc_data[1] |= 0x8;
}

bool BSP_RTC_Test_Bsy(void)
{
  return (_rtc_data[1] & 0x4) != 0;
}

void BSP_RTC_Enter_Powersave(void)
{ 
  // 停止32KHZ输出
  BSP_RTC_Read_Data(RTC_TYPE_CTL);
  BSP_RTC_Set_En32khz(0);
  BSP_RTC_Write_Data(RTC_TYPE_CTL);
  
}

void BSP_RTC_Leave_Powersave(void)
{
  // 启动32KHZ输出
  BSP_RTC_Read_Data(RTC_TYPE_CTL);
  BSP_RTC_Set_En32khz(1);
  BSP_RTC_Write_Data(RTC_TYPE_CTL);
  
}