#ifndef __IVCLOCK_EXT_H__
#define __IVCLOCK_EXT_H__

#include <stdint.h>

#define is_leap_year(y) \
(( ((y%100) !=0) && ((y%4)==0)) || ( (y%400) == 0))

typedef enum {FALSE = 0,TRUE = !FALSE} bool;

// 计算某年某月某日星期几,  经典的Zeller公式
// year 1901 - 2099
// mon 0-11
// date 0-30
// return 0-6, 0 is monday, 6 is sunday
uint8_t cext_yymmdd_to_day(uint16_t year, uint8_t mon, uint8_t date);
bool cext_cal_hour12(uint8_t hour, uint8_t * hour12);

#endif
