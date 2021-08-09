#include "display.h"
#include "iv18.h"
#include "debug.h"
#include "power.h"
#include "tim.h"
#include "adc.h"
#include "config.h"
#include "console.h"
#include "delay.h"
#include "alarm.h"

static bool _display_is_on;
static bool _display_mon_light;
static enum display_mode _display_mode;
void display_init(void)
{
  BSP_TIM1_Stop_PMW(TIM_CHANNEL_1); // light control
  _display_is_on = FALSE;
  _display_mon_light = FALSE;
  _display_mode = DISPLAY_MODE_NONE;
}

void display_format_clock(struct clock_struct * clk)
{
  if(_display_mode == DISPLAY_MODE_CLOCK_HHMMSS) {
    BSP_IV18_Set_Dig(1, '=');
    if(clk->is12) {
      clk->ispm ? BSP_IV18_Set_DP(0) : BSP_IV18_Clr_DP(0);
      BSP_IV18_Set_Dig(2, (clk->hour12 / 10) == 0 ? BSP_IV18_BLANK : (clk->hour12 / 10 + 0x30));
      BSP_IV18_Set_Dig(3, (clk->hour12 % 10 + 0x30));      
    } else {
      BSP_IV18_Set_Dig(2, (clk->hour / 10 + 0x30));
      BSP_IV18_Set_Dig(3, (clk->hour % 10 + 0x30)); 
    }
    clk->sec % 2 ? BSP_IV18_Set_DP(3) : BSP_IV18_Clr_DP(3);
    
    BSP_IV18_Set_Dig(4, (clk->min / 10 + 0x30));
    BSP_IV18_Set_Dig(5, (clk->min % 10 + 0x30)); 
    
    clk->sec % 2 ? BSP_IV18_Set_DP(5) : BSP_IV18_Clr_DP(5);
    
    BSP_IV18_Set_Dig(6, (clk->sec / 10 + 0x30));
    BSP_IV18_Set_Dig(7, (clk->sec % 10 + 0x30));
    BSP_IV18_Set_Dig(8, '=');
  } else if(_display_mode == DISPLAY_MODE_CLOCK_YYMMDD) {
    BSP_IV18_Set_Dig(1, (clk->year / 10 + 0x30));
    BSP_IV18_Set_Dig(2, (clk->year % 10 + 0x30)); 
    BSP_IV18_Set_Dig(3, '-');  
    BSP_IV18_Set_Dig(4, ((clk->mon + 1) / 10 + 0x30));
    BSP_IV18_Set_Dig(5, ((clk->mon + 1) % 10 + 0x30)); 
    BSP_IV18_Set_Dig(6, '-');  
    BSP_IV18_Set_Dig(7, ((clk->date + 1) / 10 + 0x30));
    BSP_IV18_Set_Dig(8, ((clk->date + 1) % 10 + 0x30));     
  } else if(_display_mode == DISPLAY_MODE_CLOCK_WEEK) {
    BSP_IV18_Set_Dig(2, 'D');
    BSP_IV18_Set_Dig(3, 'A');
    BSP_IV18_Set_Dig(4, 'Y');  
    BSP_IV18_Set_Dig(5, '-'); 
    BSP_IV18_Set_Dig(6, '-'); 
    BSP_IV18_Set_Dig(7, (clk->day + 1) + 0x30);     
  }
}

void display_format_timer(struct timer_struct * tmr)
{
  uint8_t ms10 = (uint8_t)(((float)(tmr[0].ms39) * 3.9) / 10);
  if(_display_mode == DISPLAY_MODE_TIMER_HHMMSS) {
    BSP_IV18_Set_Dig(2, (tmr->hour / 10 + 0x30));
    BSP_IV18_Set_Dig(3, (tmr->hour % 10 + 0x30)); 
    BSP_IV18_Set_DP(3);
    BSP_IV18_Set_Dig(4, (tmr->min / 10 + 0x30));
    BSP_IV18_Set_Dig(5, (tmr->min % 10 + 0x30)); 
    BSP_IV18_Set_DP(5); 
    BSP_IV18_Set_Dig(6, (tmr->sec / 10 + 0x30));
    BSP_IV18_Set_Dig(7, (tmr->sec % 10 + 0x30)); 
  } else if(_display_mode == DISPLAY_MODE_TIMER_MMSSMM) {
    BSP_IV18_Set_Dig(2, (tmr->min / 10 + 0x30));
    BSP_IV18_Set_Dig(3, (tmr->min % 10 + 0x30)); 
    BSP_IV18_Set_DP(3);
    BSP_IV18_Set_Dig(4, (tmr->sec / 10 + 0x30));
    BSP_IV18_Set_Dig(5, (tmr->sec % 10 + 0x30)); 
    BSP_IV18_Set_DP(5); 
    BSP_IV18_Set_Dig(6, (ms10 / 10 + 0x30));
    BSP_IV18_Set_Dig(7, (ms10 % 10 + 0x30));
  }
}

void display_format_alarm0(struct alarm0_struct * alarm0)
{
  bool ispm;
  uint8_t hour12, i;
  
  if(_display_mode == DISPLAY_MODE_ALARM_HHMM) {
    BSP_IV18_Set_Dig(2,'A');
    BSP_IV18_Set_Dig(3,'L');
    BSP_IV18_Set_Dig(4,'-');
    ispm = cext_cal_hour12(alarm0->hour, &hour12);
    if(config_read("time_12")->val8) {
      ispm ? BSP_IV18_Set_DP(0) : BSP_IV18_Clr_DP(0);
      BSP_IV18_Set_Dig(5, (hour12 / 10) == 0 ? BSP_IV18_BLANK : (hour12 / 10 + 0x30));
      BSP_IV18_Set_Dig(6, (hour12 % 10 + 0x30)); 
    } else {
      BSP_IV18_Set_Dig(5, (alarm0->hour / 10 + 0x30));
      BSP_IV18_Set_Dig(6, (alarm0->hour % 10 + 0x30));
    }
    BSP_IV18_Set_DP(6);
    BSP_IV18_Set_Dig(7, (alarm0->min / 10 + 0x30));
    BSP_IV18_Set_Dig(8, (alarm0->min % 10 + 0x30));
  } else if(_display_mode == DISPLAY_MODE_ALARM_DAY) {
    for(i = 0 ; i < 7 ; i++) {
      BSP_IV18_Set_Dig(i + 1, alarm0_test_enable(i + 1) ? '0' : '-');
    }
  } else if(_display_mode == DISPLAY_MODE_ALARM_SND) {
    BSP_IV18_Set_Dig(1, 'S');
    BSP_IV18_Set_Dig(2, 'N');
    BSP_IV18_Set_Dig(3, 'D'); 
    BSP_IV18_Set_Dig(4, '-');
    BSP_IV18_Set_Dig(5, alarm0->snd + 0x30);
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
  BSP_TIM4_Start();
  _display_is_on = TRUE;
  _display_mon_light = config_read("mon_lt_en")->val8;
  if(_display_mon_light) {
    BSP_TIM1_Start_PMW(TIM_CHANNEL_1); // light control
  }
}

void display_off(void)
{
  power_iv18_enable(FALSE);
  power_490_enable(FALSE);
  if(_display_mon_light) {
    BSP_TIM1_Stop_PMW(TIM_CHANNEL_1); 
  }
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

void display_clr(void)
{
  BSP_IV18_Clr();
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
  config_write("mon_lt_en", &val);
  if(enable)
    BSP_TIM1_Start_PMW(TIM_CHANNEL_1);
  else
    BSP_TIM1_Stop_PMW(TIM_CHANNEL_1);
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

void display_enter_powersave(void)
{
  display_clr();
  delay_ms(10);
  display_off();
}

void display_leave_powersave(void)
{
  display_on();
}

void display_set_mode(enum display_mode mode)
{
  _display_mode = mode;
}

enum display_mode display_get_mode(void)
{
  return _display_mode;
}

void display_set_blink_clock_hour(bool enable)
{
  if(enable) {
    display_set_blink(2);
    display_set_blink(3);    
  } else {
    display_clr_blink(2);
    display_clr_blink(3); 
  }
}

void display_set_blink_clock_min(bool enable)
{
  if(enable) {
    display_set_blink(4);
    display_set_blink(5);    
  } else {
    display_clr_blink(4);
    display_clr_blink(5); 
  }
}

void display_set_blink_clock_sec(bool enable)
{
  if(enable) {
    display_set_blink(6);
    display_set_blink(7);    
  } else {
    display_clr_blink(6);
    display_clr_blink(7); 
  }
}

void display_set_blink_clock_year(bool enable)
{
  if(enable) {
    display_set_blink(1);
    display_set_blink(2);    
  } else {
    display_clr_blink(1);
    display_clr_blink(2); 
  }
}

void display_set_blink_clock_mon(bool enable)
{
  if(enable) {
    display_set_blink(4);
    display_set_blink(5);    
  } else {
    display_clr_blink(4);
    display_clr_blink(5); 
  }
}

void display_set_blink_clock_date(bool enable)
{
  if(enable) {
    display_set_blink(7);
    display_set_blink(8);    
  } else {
    display_clr_blink(7);
    display_clr_blink(8); 
  }
}

void display_set_blink_alarm_hour(bool enable)
{
  if(enable) {
    display_set_blink(5);
    display_set_blink(6);    
  } else {
    display_clr_blink(5);
    display_clr_blink(6); 
  }
}

void display_set_blink_alarm_min(bool enable)
{
  if(enable) {
    display_set_blink(7);
    display_set_blink(8);    
  } else {
    display_clr_blink(7);
    display_clr_blink(8); 
  }
}

// day 1 - 7
void display_set_blink_alarm_day(bool enable, uint8_t day)
{
  if(enable) {
    display_set_blink(day);
    display_set_blink(day);    
  } else {
    display_clr_blink(day);
    display_clr_blink(day); 
  }
}

void display_set_blink_alarm_snd(bool enable)
{
  if(enable) {
    display_set_blink(5);    
  } else {
    display_clr_blink(5); 
  }
}
