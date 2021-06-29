#include "cext.h"

// 计算某年某月某日星期几,  经典的Zeller公式
// year 0-99
// mon 0-11
// date 0-30
// return 0-6, 0 is monday, 6 is sunday
unsigned char cext_yymmdd_to_day(unsigned char year, unsigned char mon, unsigned char date)
{
  unsigned int d,m,y,c;
  d = date + 1;
  m = mon + 1;
  y = CEXT_YEAR_BASE + year;
  
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

// year is 0-99 (2000 - 2099)
char cext_is_leap_year(unsigned char year)
{
  unsigned int y;
  if(year >= 100) year = 99;
  y = year + CEXT_YEAR_BASE;
  return is_leap_year(y);
}