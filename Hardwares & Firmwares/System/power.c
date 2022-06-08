#include "power.h"
#include "cext.h"
#include "debug.h"
#include "gpio.h"
#include "adc.h"
#include "tim.h"
#include "config.h"
#include "console.h"
#include "pwr.h"
#include "clock.h"
#include "player.h"
#include "ocv.h"
#include "display.h"

static bool power_is_in_powersave;
static uint8_t curr_power_save_dur;
static uint8_t power_save_timeo;

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
  
  cal65 = config_read_int("bat_65");

  power_50_enable(TRUE);
  power_wdg_set(cal65, 0xFFF);
  power_mon_start();
  
  power_is_in_powersave = FALSE;
  power_save_timeo = config_read_int("power_timeo");
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

void power_player_enable(bool enable)
{
  power_50_enable(enable);
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

bool power_player_enabled(void)
{
  return power_50_enabled();
}

float power_get_bat_voltage(void)
{
  uint16_t val, cal65, cal90;
  float voltage, k, b;
  cal65 = config_read_int("bat_65");
  cal90 = config_read_int("bat_90");  
  val = BSP_ADC1_Get_Value();

  k = (9.0 - 6.5) / (cal90 - cal65);
  b = 9.0 - k * cal90;
  voltage = k * val + b;
  IVDBG("power_get_bat_voltage val = %d, k = %f, b = %f, v = %f", val, k, b, voltage);
  return voltage;
}

uint8_t power_get_bat_quantity(void)
{
  float vol = power_get_bat_voltage();
  uint8_t i;
  for(i = 0 ; i < sizeof(ocv_table)/sizeof(ocv_t) ; i++) {
    if(vol < ocv_table[i].voltage * 2)
      break;
  }
  
  if(i == sizeof(ocv_table)/sizeof(ocv_t))
    return 100;
  else if(i == 0)
    return 0;
  
  return ocv_table[i - 1].quantity +
    (vol / 2.0 -  ocv_table[i - 1].voltage) *
    ((ocv_table[i].quantity - ocv_table[i-1].quantity) /
    (ocv_table[i].voltage - ocv_table[i - 1].voltage));
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

void power_enter_powersave(void)
{
  IVDBG("power_enter_powersave");
  
  clock_enter_powersave();
  display_enter_powersave();
  player_enter_powersave();
  
  power_is_in_powersave = TRUE;
  
  while(power_is_in_powersave) {
    BSP_PWR_Sleep();
  }
 
  clock_leave_powersave();
  display_leave_powersave();
  player_leave_powersave();
  power_reset_timeo();  
}

void power_wakeup(void)
{
  power_is_in_powersave = FALSE;
}

void power_reset_timeo(void)
{
  curr_power_save_dur = clock_get_now_sec();
}

void power_set_timeo(uint8_t timeo)
{
  uint8_t r = power_save_timeo / POWER_STEP_TIMEO;
  power_save_timeo = r * POWER_STEP_TIMEO;
  if(power_save_timeo > POWER_MAX_TIMEO)
    power_save_timeo = 0;
}

uint8_t power_get_timeo(void)
{
  return power_save_timeo;
}

void power_timeo_save_config(void)
{
  config_val_t val;
  val.val8 = power_save_timeo;
  config_write("power_timeo", &val);
}

void power_inc_timeo(void)
{
  config_val_t val;
  power_save_timeo += POWER_STEP_TIMEO;
  if(power_save_timeo > POWER_MAX_TIMEO)
    power_save_timeo = 0;
}

void power_test_powersave(void)
{
  uint8_t diff;
  if(power_save_timeo == 0)
    return;
  diff = clock_diff_now_sec(curr_power_save_dur);
  IVDBG("power_test_powersave diff = %d", diff);
  if(diff > power_save_timeo) {
    power_enter_powersave();
  }
}

void power_scan(void)
{
  task_set(EV_POWEROFF);  
}


