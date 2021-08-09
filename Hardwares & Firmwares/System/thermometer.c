#include "thermometer.h"
#include "ds3231.h"
#include "display.h"
#include "config.h"

void thermometer_init(void)
{

}

bool thermometer_read_fah(uint16_t * integer, uint16_t * flt)
{
  bool sign;
  double res;
  uint8_t _integer, _flt;
  BSP_DS3231_Read_Data(BSP_DS3231_TYPE_TEMP);
  sign = BSP_DS3231_Get_Temperature(&_integer, &_flt);
  
  res = ((double)(*integer) + (double)(*flt) / 100) * 33.8;
  *integer = (uint16_t) res;
  *flt     = (uint16_t)((res * 100)) % 100;
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

void thermometer_display(void)
{
  uint16_t integer, flt;
  bool sign;
  
  if(config_read("temp_cen")->val8) {
    sign = thermometer_read_cen(&integer, &flt);
  } else {
    sign = thermometer_read_fah(&integer, &flt);
  }
  
  display_clr();
  
  if(integer / 100)
    display_set_dig(1, (integer / 100) % 10 + 0x30);  
  
  if((integer % 100) / 10 || integer / 100)
    display_set_dig(2, (integer % 100) / 10 + 0x30);
  
  display_set_dig(3, integer % 10 + 0x30);
  display_set_dp(3);
  display_set_dig(4, flt / 10 + 0x30);
  display_set_dig(5, flt % 10 + 0x30);
  
  display_set_dig(7, config_read("temp_cen")->val8 ? 'C' : 'F'); 
  display_set_dig(8, DISPLAY_DEGREE);  
}