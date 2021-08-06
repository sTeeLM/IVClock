#ifndef __IVCLOCK_THERMOMETER_H__
#define __IVCLOCK_THERMOMETER_H__

#include <stdint.h>
#include "cext.h"

void thermometer_init(void);
bool thermometer_read_cen(uint16_t * integer, uint16_t * flt);
bool thermometer_read_fah(uint16_t * integer, uint16_t * flt);
void thermometer_display(void);

#endif
