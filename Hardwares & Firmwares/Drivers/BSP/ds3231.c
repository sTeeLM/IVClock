#include "ds3231.h"
#include "bsp.h"
#include "debug.h"
#include "i2c.h"
#include "key.h"
#include "delay.h"
#include "cext.h"

#include <string.h>

#define BSP_DS3231_I2C_ADDRESS  0xD0 //11010000

#define BSP_DS3231_TIME_OFFSET 0x00
#define BSP_DS3231_DATE_OFFSET 0x03
#define BSP_DS3231_ALARM0_OFFSET 0x07
#define BSP_DS3231_ALARM1_OFFSET 0x0B
#define BSP_DS3231_CTL_OFFSET 0x0E
#define BSP_DS3231_TEMP_OFFSET 0x11

#define BSP_DS3231_CTL_OFFSET 0x0E

static uint8_t _ds3231_data[4];
static uint8_t _last_read;
static uint8_t _is_lt_timer_mode;

static const char * ds3231_alarm_mode_str[] = 
{
  "BSP_DS3231_ALARM0_MOD_PER_SEC",
  "BSP_DS3231_ALARM0_MOD_MATCH_SEC",  
  "BSP_DS3231_ALARM0_MOD_MATCH_MIN_SEC", 
  "BSP_DS3231_ALARM0_MOD_MATCH_HOUR_MIN_SEC", 
  "BSP_DS3231_ALARM0_MOD_MATCH_DATE_HOUR_MIN_SEC",   
  "BSP_DS3231_ALARM0_MOD_MATCH_DAY_HOUR_MIN_SEC",
  "BSP_DS3231_ALARM0_MOD_CNT", 
  "BSP_DS3231_ALARM1_MOD_PER_MIN",
  "BSP_DS3231_ALARM1_MOD_MATCH_MIN",  
  "BSP_DS3231_ALARM1_MOD_MATCH_HOUR_MIN", 
  "BSP_DS3231_ALARM1_MOD_MATCH_DATE_HOUR_MIN",   
  "BSP_DS3231_ALARM1_MOD_MATCH_DAY_HOUR_MIN", 
};

static const char * ds3231_square_rate_str[] = 
{
  "BSP_DS3231_SQUARE_RATE_1HZ",
  "BSP_DS3231_SQUARE_RATE_1024HZ",
  "BSP_DS3231_SQUARE_RATE_4096HZ",
  "BSP_DS3231_SQUARE_RATE_8192HZ"
};

static void _ds3231_dump_raw(void)
{
  uint8_t addr;
  uint8_t c;
  BSP_Error_Type ret;
  IVDBG("DS3231 raw content:");
  for(addr = 0; addr < 0x12; addr ++) {
    BSP_I2C_Read(BSP_DS3231_I2C_ADDRESS, addr, I2C_MEMADD_SIZE_8BIT, &c, 1);
    IVDBG("ds3231 [%02x] = 0x%02x", addr, c);
  }
}

static void _ds3231_initialize (void)
{
  uint8_t count;

  IVDBG("DS3231 before initialize:");
  _ds3231_dump_raw();
  BSP_DS3231_Dump();
  
  _is_lt_timer_mode = 0;
    
  memset(_ds3231_data, 0, sizeof(_ds3231_data));

  delay_ms(10);
  
  // RTC内部使用24小时制
  BSP_DS3231_Read_Data(BSP_DS3231_TYPE_TIME); 
  if(BSP_DS3231_Time_Get_Hour_12()) {
    IVDBG("DS3231 set time format to 24");
    count = BSP_DS3231_Time_Get_Hour();
    BSP_DS3231_Time_Set_Hour_12(FALSE);
    BSP_DS3231_Time_Set_Hour(count);
    BSP_DS3231_Write_Data(BSP_DS3231_TYPE_TIME); 
  }    
  
  // 闹钟设置为24小时格式
  BSP_DS3231_Read_Data(BSP_DS3231_TYPE_ALARM0);
  if(BSP_DS3231_Alarm_Get_Hour_12()) {
    IVDBG("DS3231 set alarm0 format to 24");
    BSP_DS3231_Alarm_Set_Hour_12(FALSE);
    BSP_DS3231_Write_Data(BSP_DS3231_TYPE_ALARM0);  
  }
  
  BSP_DS3231_Read_Data(BSP_DS3231_TYPE_ALARM1);
  if(BSP_DS3231_Alarm_Get_Hour_12()) {
    IVDBG("DS3231 set alarm1 format to 24");
    BSP_DS3231_Alarm_Set_Hour_12(FALSE);
    BSP_DS3231_Write_Data(BSP_DS3231_TYPE_ALARM1);
  }

  // 清除所有闹钟：闹钟配置由alarm自行从rom中读取，写入rtc
  BSP_DS3231_Read_Data(BSP_DS3231_TYPE_CTL);
  BSP_DS3231_Enable_Alarm_Int(BSP_DS3231_ALARM0, 0);
  BSP_DS3231_Clr_Alarm_Int_Flag(BSP_DS3231_ALARM0);
  BSP_DS3231_Enable_Alarm_Int(BSP_DS3231_ALARM1, 0);
  BSP_DS3231_Clr_Alarm_Int_Flag(BSP_DS3231_ALARM1);
  // 允许RTC发中断
  BSP_DS3231_Set_Intcn(1);
  // 启动32KHZ输出  
  BSP_DS3231_Set_En32khz(1);
  // 清除eosc
  BSP_DS3231_Set_Eosc(0);
  // BBSQW = 0，电池供电时候没有方波或者中断
  BSP_DS3231_Set_Bbsqw(0);
  BSP_DS3231_Write_Data(BSP_DS3231_TYPE_CTL); 
  
  IVDBG("DS3231 after initialize:");
  _ds3231_dump_raw();
  BSP_DS3231_Dump();
}

BSP_Error_Type BSP_DS3231_Init(void)
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
  
  _ds3231_initialize();

  return BSP_ERROR_NONE;
}


void BSP_DS3231_Dump(void)
{
  IVDBG("BSP_DS3231_Dump:");
  BSP_DS3231_Read_Data(BSP_DS3231_TYPE_DATE);
  IVDBG("date/day: (%02u)%02u-%02u-%02u/%u",
    BSP_DS3231_Date_Get_Centry(),
    BSP_DS3231_Date_Get_Year(), BSP_DS3231_Date_Get_Month(), BSP_DS3231_Date_Get_Date(),
    BSP_DS3231_Date_Get_Day()
  );
  
  BSP_DS3231_Read_Data(BSP_DS3231_TYPE_TIME);
  IVDBG("time: %02u:%02u:%02u, is12: %s",
    BSP_DS3231_Time_Get_Hour(), BSP_DS3231_Time_Get_Min(), BSP_DS3231_Time_Get_Sec(),
    BSP_DS3231_Time_Get_Hour_12() ? "ON" : "OFF"
  );
  
  BSP_DS3231_Read_Data(BSP_DS3231_TYPE_ALARM0);
  IVDBG("alarm0 mode: %s", BSP_DS3231_Alarm_Get_Mode_Str());
  IVDBG("  day:%02u", BSP_DS3231_Alarm_Get_Day());
  IVDBG("  date:%02u", BSP_DS3231_Alarm_Get_Date());  
  IVDBG("  hour:%02u", BSP_DS3231_Alarm_Get_Hour());
  IVDBG("  min:%02u", BSP_DS3231_Alarm_Get_Min());  
  IVDBG("  sec:%02u", BSP_DS3231_Alarm_Get_Sec());
  IVDBG("  is12:%s", BSP_DS3231_Alarm_Get_Hour_12() ? "ON" : "OFF");  

  BSP_DS3231_Read_Data(BSP_DS3231_TYPE_ALARM1);
  IVDBG("alarm1 mode: %s", BSP_DS3231_Alarm_Get_Mode_Str());
  IVDBG("  day:%02u", BSP_DS3231_Alarm_Get_Day());
  IVDBG("  date:%02u", BSP_DS3231_Alarm_Get_Date());  
  IVDBG("  hour:%02u", BSP_DS3231_Alarm_Get_Hour());
  IVDBG("  min:%02u", BSP_DS3231_Alarm_Get_Min());  
  IVDBG("  is12:%s", BSP_DS3231_Alarm_Get_Hour_12() ? "ON" : "OFF");
  
  BSP_DS3231_Read_Data(BSP_DS3231_TYPE_CTL);
  IVDBG("control:");
  IVDBG("  alarm0 int enable:%s", BSP_DS3231_Test_Alarm_Int(BSP_DS3231_ALARM0) ? "ON" : "OFF");
  IVDBG("  alarm1 int enable:%s", BSP_DS3231_Test_Alarm_Int(BSP_DS3231_ALARM1) ? "ON" : "OFF");
  IVDBG("  alarm0 int flag:%s", BSP_DS3231_Test_Alarm_Int_Flag(BSP_DS3231_ALARM0) ? "ON" : "OFF");
  IVDBG("  alarm1 int flag:%s", BSP_DS3231_Test_Alarm_Int_Flag(BSP_DS3231_ALARM1) ? "ON" : "OFF");
  IVDBG("  eosc:%c", BSP_DS3231_Test_Eosc() ? '1' : '0');  
  IVDBG("  bbsqw:%c", BSP_DS3231_Test_Bbsqw() ? '1' : '0');  
  IVDBG("  conv:%c", BSP_DS3231_Test_Conv() ? '1' : '0');  
  IVDBG("  square_rate:%s", BSP_DS3231_Get_Square_Rate_Str());
  IVDBG("  intcn:%c", BSP_DS3231_Test_Intcn() ? '1' : '0');  
  IVDBG("  osf:%c", BSP_DS3231_Test_Osf() ? '1' : '0');  
  IVDBG("  en32khz:%c", BSP_DS3231_Test_En32khz() ? '1' : '0');  
  IVDBG("  bsy:%c", BSP_DS3231_Test_Bsy() ? '1' : '0');  
}

bool BSP_DS3231_Is_Lt_Timer(void)
{
  return _is_lt_timer_mode;
}

void BSP_DS3231_Set_Lt_Timer(bool enable)
{
  _is_lt_timer_mode = enable;
}

void BSP_DS3231_Read_Data(enum BSP_DS3231_Data_Type type)
{
  uint8_t offset;
  switch(type) {
    case BSP_DS3231_TYPE_TIME:
      offset = BSP_DS3231_TIME_OFFSET; break;
    case BSP_DS3231_TYPE_DATE:
      offset = BSP_DS3231_DATE_OFFSET; break;    
    case BSP_DS3231_TYPE_ALARM0:
      offset = BSP_DS3231_ALARM0_OFFSET; break;
    case BSP_DS3231_TYPE_ALARM1:
      offset = BSP_DS3231_ALARM1_OFFSET; break;
    case BSP_DS3231_TYPE_TEMP:
      offset = BSP_DS3231_TEMP_OFFSET; break; 
    case BSP_DS3231_TYPE_CTL:
      offset = BSP_DS3231_CTL_OFFSET; break;     
  }
  
  _last_read = type;
  
  BSP_I2C_Read(BSP_DS3231_I2C_ADDRESS, offset, I2C_MEMADD_SIZE_8BIT, _ds3231_data, sizeof(_ds3231_data));
  
}

void BSP_DS3231_Write_Data(enum BSP_DS3231_Data_Type type)
{
  uint8_t offset;
  switch(type) {
    case BSP_DS3231_TYPE_TIME:
      offset = BSP_DS3231_TIME_OFFSET; break;
    case BSP_DS3231_TYPE_DATE:
      offset = BSP_DS3231_DATE_OFFSET; break;    
    case BSP_DS3231_TYPE_ALARM0:
      offset = BSP_DS3231_ALARM0_OFFSET; break;
    case BSP_DS3231_TYPE_ALARM1:
      offset = BSP_DS3231_ALARM1_OFFSET; break;
    case BSP_DS3231_TYPE_TEMP:
      offset = BSP_DS3231_TEMP_OFFSET; break; 
    case BSP_DS3231_TYPE_CTL:
      offset = BSP_DS3231_CTL_OFFSET; break;       
  }
  
  BSP_I2C_Write(BSP_DS3231_I2C_ADDRESS, offset, I2C_MEMADD_SIZE_8BIT, _ds3231_data, sizeof(_ds3231_data));
   
}

static uint8_t _ds3231_get_hour(uint8_t hour)
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


static void _ds3231_set_hour(uint8_t hour, uint8_t * dat)
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

static void _ds3231_set_hour_12(bool enable, uint8_t * dat)
{
  uint8_t hour;
  hour = _ds3231_get_hour(* dat);
  if(enable) {
    *dat |= 0x40;
  } else {
    *dat &= ~0x40;
  }
  _ds3231_set_hour(hour, dat);  
}

static bool _ds3231_get_hour_12(uint8_t hour)
{
  return ((hour & 0x40) != 0);
}

static uint8_t _ds3231_get_min_sec(uint8_t min)
{
  return (min & 0x0F) + ((min & 0x70) >> 4) * 10;
}

static void _ds3231_set_min_sec(uint8_t min, uint8_t * dat)
{
  *dat &= 0xF0;
  *dat |= min % 10;
  *dat &= 0x8F;
  *dat |= (min / 10) << 4;
}

// 在read__ds3231_data(BSP_DS3231_TYPE_TIME)之后调用
// 此函数始终返回24小时格式的时间！
uint8_t BSP_DS3231_Time_Get_Hour(void)
{
  return _ds3231_get_hour(_ds3231_data[2]);
}


// 此函数时钟传入24小时格式的时间
void BSP_DS3231_Time_Set_Hour(uint8_t hour)
{
  _ds3231_set_hour(hour, &_ds3231_data[2]);
}

void BSP_DS3231_Time_Set_Hour_12(bool enable)
{
  _ds3231_set_hour_12(enable, &_ds3231_data[2]);
}

bool BSP_DS3231_Time_Get_Hour_12()
{
  return _ds3231_get_hour_12(_ds3231_data[2]);
}

uint8_t BSP_DS3231_Time_Get_Min(void)
{
  return _ds3231_get_min_sec(_ds3231_data[1]);
}

void BSP_DS3231_Time_Set_Min(uint8_t min)
{
  _ds3231_set_min_sec(min, &_ds3231_data[1]);
}

uint8_t BSP_DS3231_Time_Get_Sec(void)
{
  return _ds3231_get_min_sec(_ds3231_data[0]);
}

void BSP_DS3231_Time_Set_Sec(uint8_t sec)
{
  _ds3231_set_min_sec(sec, &_ds3231_data[0]);
}

// 在BSP_DS3231_Read_Data(BSP_DS3231_TYPE_DATE)之后调用
uint8_t BSP_DS3231_Date_Get_Year()
{
  return (_ds3231_data[3] & 0x0F) + ((_ds3231_data[3] & 0xF0) >> 4) * 10;
}

bool BSP_DS3231_Date_Get_Centry()
{
  return ((_ds3231_data[2] & 0x80) != 0);
}

void BSP_DS3231_Date_Set_Centry(bool set)
{
  _ds3231_data[2] &= ~0x80;
  if(set)
    _ds3231_data[2] |= 0x80;
}

void BSP_DS3231_Date_Set_Year(uint8_t year)
{
  _ds3231_data[3] &= 0xF0;
  _ds3231_data[3] |= year % 10;
  _ds3231_data[3] &= 0x0F;
  _ds3231_data[3] |= (year / 10) << 4;  
}

uint8_t BSP_DS3231_Date_Get_Month()
{
  return (_ds3231_data[2] & 0x0F) + ((_ds3231_data[2] & 0x10) >> 4) * 10;
}

void BSP_DS3231_Date_Set_Month(uint8_t month)
{
  _ds3231_data[2] &= 0xF0;
  _ds3231_data[2] |= month % 10;
  _ds3231_data[2] &= 0x0F;
  _ds3231_data[2] |= (month / 10) << 4;  
}

static uint8_t _ds3231_get_date(uint8_t date)
{
  return (date & 0x0F) + ((date & 0x30) >> 4) * 10;
}

static void _ds3231_set_date(uint8_t date, uint8_t * dat)
{
  *dat &= 0xF0;
  *dat |= date % 10;
  *dat &= 0xCF;
  *dat |= (date / 10) << 4; 
}

uint8_t BSP_DS3231_Date_Get_Date()
{
  return _ds3231_get_date(_ds3231_data[1]);
}

// 此函数需要检查合法性！！
bool BSP_DS3231_Date_Set_Date(uint8_t date)
{
  uint8_t mon = BSP_DS3231_Date_Get_Month();
  bool centry = BSP_DS3231_Date_Get_Centry();
  
  IVDBG("BSP_DS3231_Date_Set_Date, valid check...");
  if(mon == 1 || mon == 3 || mon == 5 || mon == 7 
    || mon == 8 || mon == 10 || mon == 12) {
    if(date > 32) return FALSE;
  } else {
    if(date > 31) return FALSE;
    if(mon == 2 && is_leap_year(BSP_DS3231_Date_Get_Year() + centry ? 2000 : 1900)) {
      if(date > 30) return FALSE;
    } else if(mon == 2 && !is_leap_year(BSP_DS3231_Date_Get_Year() + centry ? 2000 : 1900)) {
      if(date > 29) return FALSE;
    }
  }
  
  IVDBG("BSP_DS3231_Date_Set_Date, valid check...OK");
  _ds3231_set_date(date, &_ds3231_data[1]);

  return TRUE;  
}

uint8_t BSP_DS3231_Date_Get_Day()
{
  return _ds3231_data[0];
}

void BSP_DS3231_Date_Set_Day(uint8_t day)
{
  if (day >= 1 && day <= 7) {
    _ds3231_data[0] = day;
  }
}

// BSP_DS3231_Read_Data(BSP_DS3231_TYPE_ALARM0)或者BSP_DS3231_TYPE_ALARM1之后调用
uint8_t BSP_DS3231_Alarm_Get_Hour()
{
  return _ds3231_get_hour(_last_read == BSP_DS3231_TYPE_ALARM0 ? _ds3231_data[2]:_ds3231_data[1]);
}

bool BSP_DS3231_Alarm_Get_Hour_12()
{
  return _ds3231_get_hour_12(_last_read == BSP_DS3231_TYPE_ALARM0 ? _ds3231_data[2] : _ds3231_data[1]);
}

void BSP_DS3231_Alarm_Set_Hour_12(bool enable)
{
  _ds3231_set_hour_12(enable, _last_read == BSP_DS3231_TYPE_ALARM0 ? &_ds3231_data[2] : &_ds3231_data[1]);
}

void BSP_DS3231_Alarm_Set_Hour(uint8_t hour)
{
  _ds3231_set_hour(hour, _last_read == BSP_DS3231_TYPE_ALARM0 ? &_ds3231_data[2] : &_ds3231_data[1]);
}

uint8_t BSP_DS3231_Alarm_Get_Min()
{
  return _ds3231_get_min_sec(_last_read == BSP_DS3231_TYPE_ALARM0 ? _ds3231_data[1] : _ds3231_data[0]);
}

void BSP_DS3231_Alarm_Set_Min( uint8_t min)
{
  _ds3231_set_min_sec(min, _last_read == BSP_DS3231_TYPE_ALARM0 ? &_ds3231_data[1] : &_ds3231_data[0]); 
}

uint8_t BSP_DS3231_Alarm_Get_Day(void)
{
  return _last_read == BSP_DS3231_TYPE_ALARM0 ? (_ds3231_data[3] & 0x0F) : (_ds3231_data[2] & 0x0F);
}

void BSP_DS3231_Alarm_Set_Day(uint8_t day)
{
  if(_last_read == BSP_DS3231_TYPE_ALARM0) {
    _ds3231_data[3] &= 0xF0;
    _ds3231_data[3] |= day;
  } else {
    _ds3231_data[2] &= 0xF0;
    _ds3231_data[2] |= day;
  }
}

uint8_t BSP_DS3231_Alarm_Get_Date(void)
{
  return _last_read == BSP_DS3231_TYPE_ALARM0 ?
    _ds3231_get_date(_ds3231_data[3]) : _ds3231_get_date(_ds3231_data[2]);
}

void BSP_DS3231_Alarm_Set_Date(uint8_t date)
{
  _last_read == BSP_DS3231_TYPE_ALARM0 ? _ds3231_set_date(date, &_ds3231_data[3]) : 
    _ds3231_set_date(date, &_ds3231_data[2]);
}

uint8_t BSP_DS3231_Alarm_Get_Sec(void)
{
  if(_last_read == BSP_DS3231_TYPE_ALARM0) {
    return (_ds3231_data[0] & 0x0F) + ((_ds3231_data[0] & 0x70) >> 4) * 10;
  }
  return 0;
}


void BSP_DS3231_Alarm_Set_Sec( uint8_t sec)
{
  if(_last_read == BSP_DS3231_TYPE_ALARM0) {
    _ds3231_data[0] &= 0xF0;
    _ds3231_data[0] |= sec % 10;
    _ds3231_data[0] &= 0x8F;
    _ds3231_data[0] |= ((sec / 10) << 4);    
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

enum BSP_DS3231_Alarm_Mode BSP_DS3231_Alarm_Get_Mode(void)
{
  uint8_t dy, a1m1, a1m2, a1m3, a1m4;
  if(_last_read == BSP_DS3231_TYPE_ALARM0) {
    dy = _ds3231_data[3] & 0x40;
    a1m1 = _ds3231_data[0] & 0x80;
    a1m2 = _ds3231_data[1] & 0x80;
    a1m3 = _ds3231_data[2] & 0x80;
    a1m4 = _ds3231_data[3] & 0x80;
    if(a1m1 && a1m2 && a1m3 && a1m4) {
      return BSP_DS3231_ALARM0_MOD_PER_SEC;
    } else if(!a1m1 && a1m2 && a1m3 && a1m4) {
      return BSP_DS3231_ALARM0_MOD_MATCH_SEC;
    } else if(!a1m1 && !a1m2 && a1m3  && a1m4) {
      return BSP_DS3231_ALARM0_MOD_MATCH_MIN_SEC;
    } else if(!a1m1 && !a1m2 && !a1m3  && a1m4) {
      return BSP_DS3231_ALARM0_MOD_MATCH_HOUR_MIN_SEC;
    } else if(!a1m1 && !a1m2 && !a1m3  && !a1m4 && !dy) {
      return BSP_DS3231_ALARM0_MOD_MATCH_DATE_HOUR_MIN_SEC;
    } else {
      return BSP_DS3231_ALARM0_MOD_MATCH_DAY_HOUR_MIN_SEC;
    }
  } else if(_last_read == BSP_DS3231_TYPE_ALARM1){
    dy = _ds3231_data[2] & 0x40;
    a1m2 = _ds3231_data[0] & 0x80;
    a1m3 = _ds3231_data[1] & 0x80;
    a1m4 = _ds3231_data[2] & 0x80;
    if(a1m2 && a1m3 && a1m4) {
      return BSP_DS3231_ALARM1_MOD_PER_MIN;
    } else if(!a1m2 && a1m3 && a1m4) {
      return BSP_DS3231_ALARM1_MOD_MATCH_MIN;
    } else if(!a1m2 && !a1m3  && a1m4) {
      return BSP_DS3231_ALARM1_MOD_MATCH_HOUR_MIN;
    } else if(!a1m2 && !a1m3  && !a1m4) {
      return BSP_DS3231_ALARM1_MOD_MATCH_DATE_HOUR_MIN;
    } else if(!a1m2 && !a1m3  && !a1m4 && !dy) {
      return BSP_DS3231_ALARM1_MOD_MATCH_DATE_HOUR_MIN;
    } else {
      return BSP_DS3231_ALARM1_MOD_MATCH_DAY_HOUR_MIN;
    }
  }
  return BSP_DS3231_ALARM0_MOD_CNT;
}
void BSP_DS3231_Alarm_Set_Mode(enum BSP_DS3231_Alarm_Mode mode)
{
  if(mode < BSP_DS3231_ALARM0_MOD_CNT && _last_read == BSP_DS3231_TYPE_ALARM0) {
    _ds3231_data[0] &= ~0x80;
    _ds3231_data[1] &= ~0x80;
    _ds3231_data[2] &= ~0x80;
    _ds3231_data[3] &= ~0x80; 
    _ds3231_data[3] &= ~0x40;    
    switch(mode) {
      case BSP_DS3231_ALARM0_MOD_PER_SEC:
        _ds3231_data[0] |=  0x80;
      case BSP_DS3231_ALARM0_MOD_MATCH_SEC:
        _ds3231_data[1] |=  0x80;
      case BSP_DS3231_ALARM0_MOD_MATCH_MIN_SEC:
        _ds3231_data[2] |=  0x80;
      case BSP_DS3231_ALARM0_MOD_MATCH_HOUR_MIN_SEC:
        _ds3231_data[3] |=  0x80;
      case BSP_DS3231_ALARM0_MOD_MATCH_DATE_HOUR_MIN_SEC:
        break;
      case BSP_DS3231_ALARM0_MOD_MATCH_DAY_HOUR_MIN_SEC:
        _ds3231_data[3] |= 0x40;
      default:
        ;
    }
  } else if(mode > BSP_DS3231_ALARM0_MOD_CNT && _last_read == BSP_DS3231_TYPE_ALARM1) {
    _ds3231_data[0] &= ~0x80;
    _ds3231_data[1] &= ~0x80;
    _ds3231_data[2] &= ~0x80;
    _ds3231_data[2] &= ~0x40;
    switch (mode) {
      case BSP_DS3231_ALARM1_MOD_PER_MIN:
        _ds3231_data[0] |=  0x80;
      case BSP_DS3231_ALARM1_MOD_MATCH_MIN:
        _ds3231_data[1] |=  0x80;
      case BSP_DS3231_ALARM1_MOD_MATCH_HOUR_MIN:
        _ds3231_data[2] |=  0x80;
      case BSP_DS3231_ALARM1_MOD_MATCH_DATE_HOUR_MIN:
        break;
      case BSP_DS3231_ALARM1_MOD_MATCH_DAY_HOUR_MIN:
        _ds3231_data[2] |= 0x40;
      default:
        ;
    }
  }
}

const char * BSP_DS3231_Alarm_Get_Mode_Str(void)
{
  return ds3231_alarm_mode_str[BSP_DS3231_Alarm_Get_Mode()];
}

// 在BSP_DS3231_Read_Data(BSP_DS3231_TYPE_TEMP)之后调用
bool BSP_DS3231_Get_Temperature(uint8_t * integer, uint8_t * flt)
{
  bool sign = ((_ds3231_data[0] &  0x80) != 0);
  uint16_t data;
  
  IVDBG("BSP_DS3231_Get_Temperature: 0x%02x 0x%02x", _ds3231_data[0], _ds3231_data[1]);
  
  data = _ds3231_data[0] & ~0x80;
  data <<= 2;
  data |= ((_ds3231_data[1] >>= 6) & 0x03);

  if(sign) {
    data --;
    data = ~data;
  }
  
  data &= 0x1FF;
  
  *integer = (data & 0x1FC) >> 2;
  *flt     = (data & 0x3) * 25;
  
  if(*integer > 99) *integer = 99;
  
  IVDBG("BSP_DS3231_Get_Temperature %c%03d.%02d", sign ? '-':'+', *integer, *flt);
  
  return sign;
  
//  float ret = 0;
//  bool sign = ((_ds3231_data[0] &  0x80) != 0);

//  
//  if(sign) { // 是负数
//    _ds3231_data[0] = ~_ds3231_data[0];
//    _ds3231_data[1] &= 0xC0;
//    _ds3231_data[1] >>= 6;
//    _ds3231_data[1] = ~_ds3231_data[1] + 1;
//    _ds3231_data[1] &= 0x03;
//    if(_ds3231_data[1] == 0) {
//      _ds3231_data[0] ++;
//    }
//  } else { //是正数
//    _ds3231_data[1] &= 0xC0;
//    _ds3231_data[1] >>= 6;
//  }
//  
//  *integer = _ds3231_data[0];
//  
//  if((_ds3231_data[1] & 0x3) == 0x3) {
//    *flt = 75;
//  } else if((_ds3231_data[1] & 0x3) == 0x2) { 
//    *flt = 50;
//  } else if((_ds3231_data[1] & 0x3) == 0x1) { 
//    *flt = 25;
//  } else {
//    *flt = 0;
//  }

//  if(*integer > 99) *integer = 99;
// 
//  return sign;
}

// 在BSP_DS3231_Read_Data（BSP_DS3231_TYPE_CTL）之后调用
void BSP_DS3231_Enable_Alarm_Int(enum BSP_DS3231_Alarm_Index index, bool enable)
{
  if(index == BSP_DS3231_ALARM0) {
    if(!enable)
      _ds3231_data[0] &= ~1;
    else
      _ds3231_data[0] |= 1;
  } else if(index == BSP_DS3231_ALARM1) {
    if(!enable)
      _ds3231_data[0] &= ~2;
    else
      _ds3231_data[0] |= 2;
  }
}

bool BSP_DS3231_Test_Alarm_Int(enum BSP_DS3231_Alarm_Index index)
{
  if(index == BSP_DS3231_ALARM0) {
    return (_ds3231_data[0] & 1) == 1;
  } else if(index == BSP_DS3231_ALARM1) {
    return (_ds3231_data[0] & 2) == 2;
  }
  return 0;
}

void BSP_DS3231_Clr_Alarm_Int_Flag(enum BSP_DS3231_Alarm_Index index)
{
  if(index == BSP_DS3231_ALARM0) {
    _ds3231_data[1] &= ~1;
  } else if(index == BSP_DS3231_ALARM1) {
    _ds3231_data[1] &= ~2;    
  } 
}

bool BSP_DS3231_Test_Alarm_Int_Flag(enum BSP_DS3231_Alarm_Index index)
{
  if(index == BSP_DS3231_ALARM0) {
    return (_ds3231_data[1] & 1) == 1;
  } else if(index == BSP_DS3231_ALARM1) {
    return (_ds3231_data[1] & 2) == 2;
  }
  return 0;
}

bool BSP_DS3231_Test_Eosc(void)
{
  return (_ds3231_data[0] & 0x80) != 0;
}

void BSP_DS3231_Set_Eosc(bool val)
{
  _ds3231_data[0] &= ~0x80;
  if(val)
    _ds3231_data[0] |= 0x80;
}

bool BSP_DS3231_Test_Bbsqw(void)
{
  return (_ds3231_data[0] & 0x40) != 0;
}

void BSP_DS3231_Set_Bbsqw(bool val)
{
  _ds3231_data[0] &= ~0x40;
  if(val)
    _ds3231_data[0] |= 0x40;
}

bool BSP_DS3231_Test_Conv(void)
{
  return (_ds3231_data[0] & 0x20) != 0;
}

void BSP_DS3231_Set_conv(bool val)
{
  _ds3231_data[0] &= ~0x20;
  if(val)
    _ds3231_data[0] |= 0x20;
}

enum BSP_DS3231_Square_Rate BSP_DS3231_Get_Square_Rate(void)
{
  return (((_ds3231_data[0] & 0x18) >> 3) & 0x3);
}

void BSP_DS3231_Set_Square_Rate(enum BSP_DS3231_Square_Rate rt)
{
  uint8_t val = rt;
  _ds3231_data[0] &= ~0x18;
  _ds3231_data[0] |= val << 3;
}

const char * BSP_DS3231_Get_Square_Rate_Str(void)
{
  return ds3231_square_rate_str[BSP_DS3231_Get_Square_Rate()];
}

bool BSP_DS3231_Test_Intcn(void)
{
  return (_ds3231_data[0] & 0x4) != 0;
}

void BSP_DS3231_Set_Intcn(bool val)
{
  _ds3231_data[0] &= ~0x4;
  if(val)
    _ds3231_data[0] |= 0x4;
}

bool BSP_DS3231_Test_Osf(void)
{
  return (_ds3231_data[1] & 0x80) != 0;
}

void BSP_DS3231_Set_Osf(bool val)
{
  _ds3231_data[1] &= ~0x80;
  if(val)
    _ds3231_data[1] |= 0x80;
}

bool BSP_DS3231_Test_En32khz(void)
{
  return (_ds3231_data[1] & 0x8) != 0;
}

void BSP_DS3231_Set_En32khz(bool val)
{
  _ds3231_data[1] &= ~0x8;
  if(val)
    _ds3231_data[1] |= 0x8;
}

bool BSP_DS3231_Test_Bsy(void)
{
  return (_ds3231_data[1] & 0x4) != 0;
}

void BSP_DS3231_Enter_Powersave(void)
{ 
  // 停止32KHZ输出
  BSP_DS3231_Read_Data(BSP_DS3231_TYPE_CTL);
  BSP_DS3231_Set_En32khz(0);
  BSP_DS3231_Write_Data(BSP_DS3231_TYPE_CTL);
  
}

void BSP_DS3231_Leave_Powersave(void)
{
  // 启动32KHZ输出
  BSP_DS3231_Read_Data(BSP_DS3231_TYPE_CTL);
  BSP_DS3231_Set_En32khz(1);
  BSP_DS3231_Write_Data(BSP_DS3231_TYPE_CTL);
  
}