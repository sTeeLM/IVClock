#ifndef __IVCLOCK_POWER_H__
#define __IVCLOCK_POWER_H__

#include <stdint.h>
#include "cext.h"

bool power_init(void);
void power_33_enable(bool enable);
void power_50_enable(bool enable);
void power_490_enable(bool enable);

#endif