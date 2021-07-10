#include "power.h"
#include "cext.h"
#include "debug.h"
#include "gpio.h"
#include "adc.h"
#include "tim.h"
#include "config.h"
#include "console.h"

void power_mon_start(void)
{
  BSP_TIM3_Start();
  BSP_ADC1_Start();
}

void power_mon_stop(void)
{
  BSP_TIM3_Stop();
  BSP_ADC1_Stop();
}

void power_wdg_set(uint16_t low, uint16_t hi)
{
  BSP_ADC1_Set_WDG(low, hi);
}

void power_init(void)
{
  uint16_t cal65, cal84;
  
  cal65 = config_read("bat_65")->val16;

  power_50_enable(TRUE);
  power_wdg_set(cal65, 0xFFF);
  power_mon_start();
}

void power_33_enable(bool enable)
{
  BSP_GPIO_Power_33_Enable(enable == TRUE ? SET : RESET);
}

void power_50_enable(bool enable)
{
  BSP_GPIO_Power_50_Enable(enable == TRUE ? SET : RESET); 
}

void power_490_enable(bool enable)
{
  BSP_GPIO_Power_490_Enable(enable == TRUE ? SET : RESET);
}

void power_iv18_enable(bool enable)
{
  BSP_GPIO_Power_IV18_Enable(enable == TRUE ? SET : RESET);
}

bool power_33_enabled(void)
{
  return BSP_GPIO_Power_33_Enabled();
}

bool power_50_enabled(void)
{
  return BSP_GPIO_Power_50_Enabled(); 
}

bool power_490_enabled(void)
{
  return BSP_GPIO_Power_490_Enabled();
}

bool power_iv18_enabled(void)
{
  return BSP_GPIO_Power_IV18_Enabled();
}

double power_get_bat_voltage(void)
{
  uint16_t val, cal65, cal90;
  double voltage, k, b;
  cal65 = config_read("bat_65")->val16;
  cal90 = config_read("bat_90")->val16;  
  val = BSP_ADC1_Get_Value();

  k = (9.0 - 6.5) / (cal90 - cal65);
  b = 9.0 - k * cal90;
  voltage = k * val + b;
  IVDBG("power_get_bat_voltage val = %d, k = %f, b = %f, v = %f", val, k, b, voltage);
  return voltage;
}

void power_cal_65(void)
{
  config_val_t val;
  val.val16 = BSP_ADC1_Get_Value();
  console_printf("6.5v -> %d\r\n", val.val16);
  config_write("bat_65", &val);
}

void power_cal_90(void)
{
  config_val_t val;
  val.val16 = BSP_ADC1_Get_Value();
  console_printf("9.0v -> %d\r\n", val.val16);
  config_write("bat_90", &val);
}

void power_scan(void)
{
  
}


