#include "thermometer.h"
#include "ds3231.h"
#include "display.h"
#include "config.h"
#include "debug.h"

static enum thermometer_unit_type thermometer_unit;

void thermometer_init(void)
{
  thermometer_unit = config_read_int("temp_cen");
}

bool thermometer_read_fah(uint16_t * integer, uint16_t * flt)
{
  bool sign;
  double res;
  uint8_t _integer, _flt;
  BSP_DS3231_Read_Data(BSP_DS3231_TYPE_TEMP);
  sign = BSP_DS3231_Get_Temperature(&_integer, &_flt);
  
  res = ((double)(_integer) + (double)(_flt) / 100) * 1.8 + 32;
  *integer = (uint16_t) res;
  *flt     = (uint16_t)((res * 100)) % 100;
  IVDBG("thermometer_read_fah %f %d %d", res, *integer, *flt);
  return sign;
}

bool thermometer_read_cen(uint16_t * integer, uint16_t * flt)
{
  uint8_t _integer, _flt;
  bool sign;
  
  BSP_DS3231_Read_Data(BSP_DS3231_TYPE_TEMP);
  sign = BSP_DS3231_Get_Temperature(&_integer, &_flt);
  *integer = _integer;
  *flt     = _flt;
  return sign;
}

double thermometer_read_cen_double(void)
{
  uint16_t _integer, _flt;
  bool sign;
  double ret;
  sign = thermometer_read_cen (&_integer, &_flt);
  ret = _integer + (double)_flt / 100.0;
  if(sign)
    ret = 0 - ret;
  return ret;
}

double thermometer_read_fah_double(void)
{
  uint16_t _integer, _flt;
  bool sign;
  double ret;
  sign = thermometer_read_fah (&_integer, &_flt);
  ret = _integer + (double)_flt / 100.0;
  if(sign)
    ret = 0 - ret;
  return ret; 
}

enum thermometer_unit_type thermometer_get_unit(void)
{
  return thermometer_unit;
}

void thermometer_set_unit(enum thermometer_unit_type unit)
{
  if(unit > THERMOMETER_UNIT_CEN)
    unit = THERMOMETER_UNIT_CEN;
  thermometer_unit = unit;
}

void thermometer_save_config(void)
{
  config_val_t val;
  val.val8 = thermometer_unit;
  config_write("temp_cen", &val);
}
