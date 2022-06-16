#include "rtc.h"
#include "cext.h"
#include "ds3231.h"

void rtc_init(void)
{
  
}

void rtc_get_time(uint8_t * hour, uint8_t * min, uint8_t * sec)
{
  BSP_DS3231_Read_Data(BSP_DS3231_TYPE_TIME);
  *hour = BSP_DS3231_Time_Get_Hour();
  *min  = BSP_DS3231_Time_Get_Min();
  *sec  = BSP_DS3231_Time_Get_Sec();  
}

void rtc_set_time(uint8_t hour, uint8_t min, uint8_t sec)
{
  BSP_DS3231_Read_Data(BSP_DS3231_TYPE_TIME);
  BSP_DS3231_Time_Set_Hour(hour);
  BSP_DS3231_Time_Set_Min(min);
  BSP_DS3231_Time_Set_Sec(sec);  
  BSP_DS3231_Write_Data(BSP_DS3231_TYPE_TIME);  
}

bool rtc_get_date(uint8_t * year, uint8_t * mon, uint8_t * date, uint8_t * day)
{
  BSP_DS3231_Read_Data(BSP_DS3231_TYPE_DATE);
  *year = BSP_DS3231_Date_Get_Year();
  *mon  = BSP_DS3231_Date_Get_Month();
  *date  = BSP_DS3231_Date_Get_Date();
  *day  = BSP_DS3231_Date_Get_Day();
  return BSP_DS3231_Date_Get_Centry();
}

void rtc_set_date(bool centry, uint8_t year, uint8_t mon, uint8_t date, uint8_t day)
{
  BSP_DS3231_Read_Data(BSP_DS3231_TYPE_DATE);
  BSP_DS3231_Date_Set_Year(year);
  BSP_DS3231_Date_Set_Month(mon);  
  BSP_DS3231_Date_Set_Date(date);
  BSP_DS3231_Date_Set_Day(day); 
  BSP_DS3231_Date_Set_Centry(centry);  
  BSP_DS3231_Write_Data(BSP_DS3231_TYPE_DATE);  
}

void rtc_enable_32768HZ(bool enable)
{
  BSP_DS3231_Read_Data(BSP_DS3231_TYPE_CTL);
  BSP_DS3231_Set_En32khz(enable);
  BSP_DS3231_Write_Data(BSP_DS3231_TYPE_CTL); 
}