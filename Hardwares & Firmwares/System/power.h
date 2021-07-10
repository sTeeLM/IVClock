#ifndef __IVCLOCK_POWER_H__
#define __IVCLOCK_POWER_H__

#include <stdint.h>
#include "cext.h"

void power_init(void);
void power_33_enable(bool enable);
void power_50_enable(bool enable);
void power_490_enable(bool enable);
bool power_33_enabled(void);
bool power_50_enabled(void);
bool power_490_enabled(void);
void power_iv18_enable(bool enable);
bool power_iv18_enabled(void);

void power_mon_start(void);
void power_mon_stop(void);
void power_wdg_set(uint16_t low, uint16_t hi);
double power_get_bat_voltage(void);
void power_cal_65(void);
void power_cal_90(void);
void power_scan(void);

#endif