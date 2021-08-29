#ifndef __IVCLOCK_MOTION_SENSOR_H__
#define __IVCLOCK_MOTION_SENSOR_H__

#include <stddef.h>
#include <stdint.h>
#include"cext.h"

void motion_sensor_init(void);

void motion_sensor_scan(void);
void motion_sensor_save_config(void);
void motion_sensor_show(void);
void motion_sensor_set_enable(bool enable);
bool motion_sensor_test_enable(void);

#endif