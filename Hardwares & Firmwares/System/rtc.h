#ifndef __IVCLOCK_RTC_H__
#define __IVCLOCK_RTC_H__

#include <stdint.h>
#include "cext.h"

void rtc_init(void);

void rtc_get_time(uint8_t * hour, uint8_t * min, uint8_t * sec);
void rtc_set_time(uint8_t hour, uint8_t min, uint8_t sec);
bool rtc_get_date(uint8_t * year, uint8_t * mon, uint8_t * date, uint8_t * day);
void rtc_set_date(bool centry, uint8_t year, uint8_t mon, uint8_t date, uint8_t day);

void rtc_enable_32768HZ(bool enable);

void rtc_dump(void);

#endif
