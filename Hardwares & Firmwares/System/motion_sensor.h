#ifndef __IVCLOCK_MOTION_SENSOR_H__
#define __IVCLOCK_MOTION_SENSOR_H__

#include <stddef.h>
#include <stdint.h>
#include"cext.h"

void motion_sensor_init(void);


void motion_sensor_scan(void);
uint8_t motion_sensor_inc_th(void);
void motion_sensor_show(void);
uint8_t motion_sensor_get_th(void);

#endif