#ifndef __IVCLOCK_CONFIG_H__
#define __IVCLOCK_CONFIG_H__

#include <stdint.h>

enum config_type {
	CONFIG_TIME_IS12,
};

void config_init(void);
uint8_t config_read(enum config_type type);

#endif