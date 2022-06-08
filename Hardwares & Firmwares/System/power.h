#ifndef __IVCLOCK_POWER_H__
#define __IVCLOCK_POWER_H__

#include <stdint.h>
#include "cext.h"

#define POWER_MAX_TIMEO 60
#define POWER_STEP_TIMEO 15
#define POWER_MIN_TIMEO POWER_STEP_TIMEO


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
float power_get_bat_voltage(void);
uint8_t power_get_bat_quantity(void);
void power_cal_65(void);
void power_cal_90(void);
void power_scan(void);

void power_enter_powersave(void);
void power_wakeup(void);
void power_test_powersave(void);
void power_reset_timeo(void);
void power_inc_timeo(void);
void power_set_timeo(uint8_t timeo);
void power_timeo_save_config(void);
uint8_t power_get_timeo(void);

void power_player_enable(bool enable);
bool power_player_enabled(void);

#endif