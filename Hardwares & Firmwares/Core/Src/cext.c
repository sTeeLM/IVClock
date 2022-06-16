#include "cext.h"

#include <stdint.h>

// 计算某年某月某日星期几,  经典的Zeller公式
// year 0-99
// mon 0-11
// date 0-30
// return 0-6, 0 is monday, 6 is sunday
uint8_t cext_yymmdd_to_day(uint16_t year, uint8_t mon, uint8_t date)
{
  unsigned int d,m,y,c;
  d = date + 1;
  m = mon + 1;
  y = year;
  
  if(m < 3){
    y -= 1;
    m += 12;
  }
  
  c = (unsigned int)(y / 100);
  y = y - 100 * c;
  
  c = (unsigned int)(c / 4) - 2 * c + y + (unsigned int) ( y / 4 ) + (26 * (m + 1) / 10) + d - 1;
  c = (c % 7 + 7) % 7;

  if(c == 0) {
    return 6;
  }
  
  return c - 1;
}
//
// 24 小时的小时到12小时的小时
bool cext_cal_hour12(uint8_t hour, uint8_t * hour12)
{
  bool ispm;
  if(hour == 0) {
    *hour12 = 12;
    ispm = FALSE;
  } else if(hour >= 1 && hour < 12) {
    *hour12 = hour;
    ispm = FALSE;
  } else if(hour == 12){
    *hour12 = hour;
    ispm = TRUE;
  } else {
    *hour12 = hour - 12;
    ispm = TRUE;
  }
  return ispm;
}