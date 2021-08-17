#ifndef __IVCLOCK_THERMOMETER_H__
#define __IVCLOCK_THERMOMETER_H__

#include <stdint.h>
#include "cext.h"

enum thermometer_unit_type
{
  THERMOMETER_UNIT_FAH,
  THERMOMETER_UNIT_CEN,
};

void thermometer_init(void);
bool thermometer_read_cen(uint16_t * integer, uint16_t * flt);
bool thermometer_read_fah(uint16_t * integer, uint16_t * flt);
enum thermometer_unit_type thermometer_get_unit(void);
void thermometer_set_unit(enum thermometer_unit_type unit);
void thermometer_save_config(void);

#endif
