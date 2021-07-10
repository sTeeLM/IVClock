#include "display.h"
#include "iv18.h"
#include "debug.h"
#include "power.h"
#include "tim.h"
#include "adc.h"
#include "config.h"
#include "console.h"

static bool _display_is_on;
static bool _display_mon_light;
  
void display_init(void)
{
  BSP_TIM1_Stop_PMW(TIM_CHANNEL_1); // light control
  _display_is_on = FALSE;
  _display_mon_light = FALSE;
}

void display_format_clock(struct clock_struct * clk, enum clock_display_mode display_mode)
{
  if(display_mode == CLOCK_DISPLAY_MODE_HHMMSS) {
    if(clk->is12) {
      clk->ispm ? BSP_IV18_Set_DP(0) : BSP_IV18_Clr_DP(0);
      BSP_IV18_Set_Dig(2, (clk->hour / 10) == 0 ? BSP_IV18_BLANK : (clk->hour / 10 + 0x30));
      BSP_IV18_Set_Dig(2, (clk->hour % 10 + 0x30));      
    } else {
      BSP_IV18_Set_Dig(2, (clk->hour / 10 + 0x30));
      BSP_IV18_Set_Dig(2, (clk->hour % 10 + 0x30)); 
    }
    BSP_IV18_Set_DP(2);
    
    BSP_IV18_Set_Dig(3, (clk->min / 10 + 0x30));
    BSP_IV18_Set_Dig(4, (clk->min % 10 + 0x30)); 
    BSP_IV18_Set_DP(4);
    
    BSP_IV18_Set_Dig(5, (clk->sec / 10 + 0x30));
    BSP_IV18_Set_Dig(6, (clk->sec % 10 + 0x30));   
  } else if(display_mode == CLOCK_DISPLAY_MODE_YYMMDD) {
    BSP_IV18_Set_Dig(1, (clk->year / 10 + 0x30));
    BSP_IV18_Set_Dig(2, (clk->year % 10 + 0x30)); 
    BSP_IV18_Set_Dig(3, '-');  
    BSP_IV18_Set_Dig(4, ((clk->mon + 1) / 10 + 0x30));
    BSP_IV18_Set_Dig(5, ((clk->mon + 1) % 10 + 0x30)); 
    BSP_IV18_Set_Dig(5, '-');  
    BSP_IV18_Set_Dig(6, ((clk->date + 1) / 10 + 0x30));
    BSP_IV18_Set_Dig(7, ((clk->date + 1) % 10 + 0x30));     
  }
}

void display_format_timer(struct timer_struct * tmr, enum timer_display_mode display_mode)
{
  uint8_t ms10 = (uint8_t)(((float)(tmr[0].ms39) * 3.9) / 10);
  if(display_mode == TIMER_DISP_MODE_HHMMSS) {
    BSP_IV18_Set_Dig(2, (tmr->hour / 10 + 0x30));
    BSP_IV18_Set_Dig(2, (tmr->hour % 10 + 0x30)); 
    BSP_IV18_Set_DP(2);
    BSP_IV18_Set_Dig(2, (tmr->min / 10 + 0x30));
    BSP_IV18_Set_Dig(2, (tmr->min % 10 + 0x30)); 
    BSP_IV18_Set_DP(2); 
    BSP_IV18_Set_Dig(2, (tmr->sec / 10 + 0x30));
    BSP_IV18_Set_Dig(2, (tmr->sec % 10 + 0x30)); 
  } else if(display_mode == TIMER_DISP_MODE_HHMMSS) {
    BSP_IV18_Set_Dig(2, (tmr->min / 10 + 0x30));
    BSP_IV18_Set_Dig(2, (tmr->min % 10 + 0x30)); 
    BSP_IV18_Set_DP(2);
    BSP_IV18_Set_Dig(2, (tmr->sec / 10 + 0x30));
    BSP_IV18_Set_Dig(2, (tmr->sec % 10 + 0x30)); 
    BSP_IV18_Set_DP(2); 
    BSP_IV18_Set_Dig(2, (ms10 / 10 + 0x30));
    BSP_IV18_Set_Dig(2, (ms10 % 10 + 0x30));
  }
}

bool display_is_on(void)
{
  return _display_is_on;
}

void display_on(void)
{
  power_iv18_enable(TRUE);
  power_490_enable(TRUE);
  BSP_TIM1_Start_PMW(TIM_CHANNEL_1); // light control
  BSP_TIM4_Start();
  _display_is_on = TRUE;
  _display_mon_light = config_read("mon_lt_on")->val8;
}

void display_off(void)
{
  power_iv18_enable(FALSE);
  power_490_enable(FALSE);
  BSP_TIM1_Stop_PMW(TIM_CHANNEL_1); 
  BSP_TIM4_Stop();
}

void display_set_dig(uint8_t index, uint8_t code)
{
  IVDBG("display_set_dig %d %c", index, code);
  BSP_IV18_Set_Dig(index, code);
}

void display_set_dp(uint8_t index)
{
  IVDBG("display_set_dp %d", index);
  BSP_IV18_Set_DP(index);  
}

void display_clr_dp(uint8_t index)
{
  IVDBG("display_clr_dp %d", index);  
  BSP_IV18_Clr_DP(index);
}

void display_set_blink(uint8_t index)
{
  IVDBG("display_set_blink %d", index);  
  BSP_IV18_Set_Blink(index);  
}

void display_clr_blink(uint8_t index)
{
  IVDBG("display_clr_blink %d", index); 
  BSP_IV18_Clr_Blink(index);  
}

// percent: 1~100
void display_set_brightness(uint8_t percent)
{
  if(percent > 100)
    percent = 100;
  if(percent == 0 )
    percent = 1;
  BSP_TIM1_Set_Duty_Cycle(TIM_CHANNEL_1, 100 - percent);
}

void display_cal_0(void)
{
  config_val_t val;
  val.val16 = BSP_ADC2_Get_Value();
  console_printf("0 -> %d\r\n", val.val16);
  config_write("lt_0", &val);
}

void display_cal_100(void)
{
  config_val_t val;
  val.val16 = BSP_ADC2_Get_Value();
  console_printf("100 -> %d\r\n", val.val16);
  config_write("lt_100", &val);
}

uint8_t display_get_light_percent(void)
{
  uint16_t val = BSP_ADC2_Get_Value();
  uint16_t lt_0 = config_read("lt_0")->val16;
  uint16_t lt_100 = config_read("lt_100")->val16; 
  double k, b;
  uint8_t percent;
  k = ((double)(100 - 0)) / ((double)(lt_100 - lt_0));
  b = 100 - k * lt_100;
  percent = (uint8_t)(k * val + b); 
  IVDBG("display_get_light_percent: val = %d, k = %f, b = %f, v = %d", val, k, b, percent);
  if(percent > 100) {
    percent = 100;
  }
  if(percent == 0) {
    percent = 0;
  }
  return (uint8_t)percent;
}

void display_mon_light_enable(bool enable)
{
  config_val_t val;
  _display_mon_light = enable;
  val.val8 = enable;
  config_write("mon_lt_on", &val);
}

bool display_mon_light_enabled(void)
{
  return _display_mon_light;
}

static uint8_t display_light_to_brightness(uint8_t light)
{
  // x0: light 100 -> y0: brightness 100
  // x1: light 0   -> y1: brightness 5
  double k, b;
  uint8_t ret;
  k = ((double)(100.0 - 5.0)) / ((double)(100.0 - 0.0));
  b = 5.0;
  ret = (uint8_t )(k * light + b);
  IVDBG("display_light_to_brightness: %d->%d", light, ret);
  return ret;
}

void display_mon_light(void)
{
  if(_display_is_on && _display_mon_light) {
    display_set_brightness(
    display_light_to_brightness(
    display_get_light_percent()));
  }
}
