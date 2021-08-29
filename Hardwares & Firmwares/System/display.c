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
#include "beeper.h"
#include "motion_sensor.h"
#include "player.h"
#include "thermometer.h"
#include <string.h>

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

// MON THU WED THU FRI SAT SUN
// 0   1   2   3   4   5   6
static const char * display_format_day(uint8_t day)
{
  switch(day) {
    case 0: 
      return "MON";
    case 1: 
      return "THU"; 
    case 2: 
      return "WED";
    case 3: 
      return "THU"; 
    case 4: 
      return "FRI";
    case 5: 
      return "SAT";
    case 6: 
      return "SUN";
    default:
      return "000";
  }
}

void display_format_clock(struct clock_struct * clk)
{
  bool ispm;
  uint8_t hour12;
  if(_display_mode == DISPLAY_MODE_CLOCK_HHMMSS) {
    ispm = cext_cal_hour12(clk->hour, &hour12);
    BSP_IV18_Set_Dig(1, '=');
    if(config_read_int("time_12")) {
      ispm ? BSP_IV18_Set_DP(0) : BSP_IV18_Clr_DP(0);
      BSP_IV18_Set_Dig(2, (hour12 / 10) == 0 ? BSP_IV18_BLANK : (hour12 / 10 + 0x30));
      BSP_IV18_Set_Dig(3, (hour12 % 10 + 0x30));      
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
    display_show_string(6, display_format_day(clk->day));    
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
  } else if(_display_mode == DISPLAY_MODE_TIMER_HHMMSSMM) {
    BSP_IV18_Set_Dig(1, (tmr->hour / 10 + 0x30));
    BSP_IV18_Set_Dig(2, (tmr->hour % 10 + 0x30)); 
    BSP_IV18_Set_DP(2);    
    BSP_IV18_Set_Dig(3, (tmr->min / 10 + 0x30));
    BSP_IV18_Set_Dig(4, (tmr->min % 10 + 0x30)); 
    BSP_IV18_Set_DP(4);
    BSP_IV18_Set_Dig(5, (tmr->sec / 10 + 0x30));
    BSP_IV18_Set_Dig(6, (tmr->sec % 10 + 0x30)); 
    BSP_IV18_Set_DP(6); 
    BSP_IV18_Set_Dig(7, (ms10 / 10 + 0x30));
    BSP_IV18_Set_Dig(8, (ms10 % 10 + 0x30));
  } else if(_display_mode == DISPLAY_MODE_TIMER_SND) {
    BSP_IV18_Set_Dig(1, 'T');
    BSP_IV18_Set_Dig(2, 'M');
    BSP_IV18_Set_Dig(3, 'R'); 
    BSP_IV18_Set_Dig(4, 'S');
    BSP_IV18_Set_Dig(5, 'N');
    BSP_IV18_Set_Dig(6, 'D');  
    BSP_IV18_Set_Dig(7, '-'); 
    BSP_IV18_Set_Dig(8, timer_get_snd() + 0x30);     
  }
}

void display_set_blink_timer_snd(bool enable)
{
  if(enable) {
    display_set_blink(8);    
  } else {
    display_clr_blink(8); 
  }
}

void display_set_blink_timer_hour(bool enable)
{
  if(enable) {
    display_set_blink(2);
    display_set_blink(3);    
  } else {
    display_clr_blink(2);
    display_clr_blink(3); 
  }
}

void display_set_blink_timer_min(bool enable)
{
  if(enable) {
    display_set_blink(4);
    display_set_blink(5);    
  } else {
    display_clr_blink(4);
    display_clr_blink(5); 
  }
}

void display_set_blink_timer_sec(bool enable)
{
  if(enable) {
    display_set_blink(6);
    display_set_blink(7);    
  } else {
    display_clr_blink(6);
    display_clr_blink(7); 
  }
}

void display_set_blink_all(bool enable)
{
  if(enable) {
    display_set_blink(1);
    display_set_blink(2);   
    display_set_blink(3);
    display_set_blink(4);
    display_set_blink(5);
    display_set_blink(6);   
    display_set_blink(7);
    display_set_blink(8);    
  } else {
    display_clr_blink(1);
    display_clr_blink(2);   
    display_clr_blink(3);
    display_clr_blink(4);
    display_clr_blink(5);
    display_clr_blink(6);   
    display_clr_blink(7);
    display_clr_blink(8);  
  }
}

void display_format_alarm0(uint8_t index)
{
  bool ispm;
  uint8_t hour12, i;
  
  if(_display_mode == DISPLAY_MODE_ALARM_HHMM) {
    BSP_IV18_Set_Dig(1,'A');
    BSP_IV18_Set_Dig(2,'L');
    BSP_IV18_Set_Dig(3, index + 0x30);     
    BSP_IV18_Set_Dig(4,'-');
    ispm = cext_cal_hour12(alarm0[index].hour, &hour12);
    if(config_read_int("time_12")) {
      ispm ? BSP_IV18_Set_DP(0) : BSP_IV18_Clr_DP(0);
      BSP_IV18_Set_Dig(5, (hour12 / 10) == 0 ? BSP_IV18_BLANK : (hour12 / 10 + 0x30));
      BSP_IV18_Set_Dig(6, (hour12 % 10 + 0x30)); 
    } else {
      BSP_IV18_Set_Dig(5, (alarm0[index].hour / 10 + 0x30));
      BSP_IV18_Set_Dig(6, (alarm0[index].hour % 10 + 0x30));
    }
    BSP_IV18_Set_DP(6);
    BSP_IV18_Set_Dig(7, (alarm0[index].min / 10 + 0x30));
    BSP_IV18_Set_Dig(8, (alarm0[index].min % 10 + 0x30));
  } else if(_display_mode == DISPLAY_MODE_ALARM_DAY) {
    for(i = 0 ; i < 7 ; i++) {
      BSP_IV18_Set_Dig(i + 1, alarm0_test_enable(index, i + 1) ? '0' : '-');
    }
  } else if(_display_mode == DISPLAY_MODE_ALARM_SND) {
    BSP_IV18_Set_Dig(1,'A');
    BSP_IV18_Set_Dig(2,'L');
    BSP_IV18_Set_Dig(3, index + 0x30);     
    BSP_IV18_Set_Dig(4,'-');
    BSP_IV18_Set_Dig(5, 'S');    
    BSP_IV18_Set_Dig(6, 'N');
    BSP_IV18_Set_Dig(7, 'D');    
    BSP_IV18_Set_Dig(8, alarm0[index].snd + 0x30);
  }
}

void display_format_alarm1(void)
{
  bool ispm;
  uint8_t hour12;  
  if(_display_mode == DISPLAY_MODE_ALARM_HHMM) {
    BSP_IV18_Set_Dig(2,'B');
    BSP_IV18_Set_Dig(3,'S');
    BSP_IV18_Set_Dig(4,'-');
    ispm = cext_cal_hour12(clock_get_hour(), &hour12);
    if(config_read_int("time_12")) {
      ispm ? BSP_IV18_Set_DP(0) : BSP_IV18_Clr_DP(0);
      BSP_IV18_Set_Dig(5, (hour12 / 10) == 0 ? BSP_IV18_BLANK : (hour12 / 10 + 0x30));
      BSP_IV18_Set_Dig(6, (hour12 % 10 + 0x30)); 
    } else {
      BSP_IV18_Set_Dig(5, (clock_get_hour() / 10 + 0x30));
      BSP_IV18_Set_Dig(6, (clock_get_hour() % 10 + 0x30));
    }
    BSP_IV18_Set_DP(6);
    BSP_IV18_Set_Dig(7, '0');
    BSP_IV18_Set_Dig(8, '0');
  } else if(_display_mode == DISPLAY_MODE_ALARM_BAOSHI) {
    BSP_IV18_Set_Dig(2,'B');
    BSP_IV18_Set_Dig(3,'S');
    BSP_IV18_Set_Dig(4,'-');
    if(alarm1_test_enable()) {
      BSP_IV18_Set_Dig(5,'O');
      BSP_IV18_Set_Dig(6,'N');
      BSP_IV18_Set_Dig(7,BSP_IV18_BLANK);    
    } else {
      BSP_IV18_Set_Dig(5,'O');
      BSP_IV18_Set_Dig(6,'F');
      BSP_IV18_Set_Dig(7,'F');      
    }
    BSP_IV18_Set_Blink(5);
    BSP_IV18_Set_Blink(6); 
    BSP_IV18_Set_Blink(7);     
  } else if(_display_mode == DISPLAY_MODE_ALARM_BAOSHI_BEGIN 
    ||_display_mode == DISPLAY_MODE_ALARM_BAOSHI_END) {
    BSP_IV18_Set_Dig(2,'B');
    BSP_IV18_Set_Dig(3,'S');
    BSP_IV18_Set_Dig(4,'-');
    
    if(_display_mode == DISPLAY_MODE_ALARM_BAOSHI_BEGIN) {
      BSP_IV18_Set_Dig(5,'B'); 
      ispm = cext_cal_hour12(alarm1_get_begin(), &hour12);
      if(config_read_int("time_12")) {
        ispm ? BSP_IV18_Set_DP(0) : BSP_IV18_Clr_DP(0);
        BSP_IV18_Set_Dig(6, hour12 / 10);
        BSP_IV18_Set_Dig(7, hour12 % 10); 
      } else {
        BSP_IV18_Set_Dig(6, alarm1_get_begin() / 10 + 0x30);
        BSP_IV18_Set_Dig(7, alarm1_get_begin() % 10 + 0x30);
      }
    } else {
      BSP_IV18_Set_Dig(5,'E'); 
      ispm = cext_cal_hour12(alarm1_get_end(), &hour12);
      if(config_read_int("time_12")) {
        ispm ? BSP_IV18_Set_DP(0) : BSP_IV18_Clr_DP(0);
        BSP_IV18_Set_Dig(6, hour12 / 10);
        BSP_IV18_Set_Dig(7, hour12 % 10); 
      } else {
        BSP_IV18_Set_Dig(6, alarm1_get_end() / 10 + 0x30);
        BSP_IV18_Set_Dig(7, alarm1_get_end() % 10 + 0x30);
      }
    }
    BSP_IV18_Set_Blink(6); 
    BSP_IV18_Set_Blink(7); 
  }
}

void display_format_param_beeper(void)
{
  BSP_IV18_Set_Dig(2,'B');
  BSP_IV18_Set_Dig(3,'P');
  BSP_IV18_Set_Dig(4,'-');
  if(beeper_test_enable()) {
    BSP_IV18_Set_Dig(5,'O');
    BSP_IV18_Set_Dig(6,'N');
    BSP_IV18_Set_Dig(7,BSP_IV18_BLANK);    
  } else {
    BSP_IV18_Set_Dig(5,'O');
    BSP_IV18_Set_Dig(6,'F');
    BSP_IV18_Set_Dig(7,'F');      
  }
  BSP_IV18_Set_Blink(5);
  BSP_IV18_Set_Blink(6); 
  BSP_IV18_Set_Blink(7); 
}
void display_format_poweroff(void)
{
  BSP_IV18_Set_Dig(1,'P');
  BSP_IV18_Set_Dig(2,'O');
  BSP_IV18_Set_Dig(3,'W');
  BSP_IV18_Set_Dig(4,'E');
  BSP_IV18_Set_Dig(5,'R');
  BSP_IV18_Set_Dig(6,'O');
  BSP_IV18_Set_Dig(7,'F');
  BSP_IV18_Set_Dig(8,'F'); 
  display_set_blink_all(TRUE);
}

void display_format_thermo(void)
{
  uint16_t integer, flt;
  bool sign;
  
  if(thermometer_get_unit() == THERMOMETER_UNIT_CEN) {
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
  
  display_set_dig(7, thermometer_get_unit() ==  THERMOMETER_UNIT_CEN? 'C' : 'F'); 
  display_set_dig(8, DISPLAY_DEGREE);  
}

void display_format_thermo_unit(void)
{
  BSP_IV18_Set_Dig(1,'T');
  BSP_IV18_Set_Dig(2,'H');
  BSP_IV18_Set_Dig(3,'E'); 
  BSP_IV18_Set_Dig(4,'R');  
  BSP_IV18_Set_Dig(5,'M'); 
  BSP_IV18_Set_Dig(6,'-');
  display_set_dig(7, thermometer_get_unit() ==  THERMOMETER_UNIT_CEN? 'C' : 'F'); 
  display_set_dig(8, DISPLAY_DEGREE);
  BSP_IV18_Set_Blink(7); 
  BSP_IV18_Set_Blink(8); 
}

void display_format_power(void)
{
  BSP_IV18_Set_Dig(2,'P');
  BSP_IV18_Set_Dig(3,'S');
  BSP_IV18_Set_Dig(4,'-');
  if(power_get_timeo() == 0) {
    BSP_IV18_Set_Dig(5,'O');
    BSP_IV18_Set_Dig(6,'F');
    BSP_IV18_Set_Dig(7,'F');    
  } else {
    BSP_IV18_Set_Dig(5, power_get_timeo() / 10 + 0x30);
    BSP_IV18_Set_Dig(6, power_get_timeo() % 10 + 0x30);
    BSP_IV18_Set_Dig(7, BSP_IV18_BLANK);      
  }
  BSP_IV18_Set_Blink(5);
  BSP_IV18_Set_Blink(6); 
  BSP_IV18_Set_Blink(7); 
}

void display_format_hour12(void)
{
  BSP_IV18_Set_Dig(2,'H');
  BSP_IV18_Set_Dig(3,'O');
  BSP_IV18_Set_Dig(4,'U');
  BSP_IV18_Set_Dig(5,'R');
  BSP_IV18_Set_Dig(6,'-');  
  if(config_read_int("time_12")) {
    BSP_IV18_Set_Dig(7,'1');
    BSP_IV18_Set_Dig(8,'2');   
  } else {
    BSP_IV18_Set_Dig(7,'2');
    BSP_IV18_Set_Dig(8,'4');     
  }
  BSP_IV18_Set_Blink(7);
  BSP_IV18_Set_Blink(8);  
}

void display_format_light_mon(void)
{
  BSP_IV18_Set_Dig(2,'L');
  BSP_IV18_Set_Dig(3,'M');
  BSP_IV18_Set_Dig(4,'-');
  if(display_mon_light_test_enable()) {
    BSP_IV18_Set_Dig(5,'O');
    BSP_IV18_Set_Dig(6,'N');
    BSP_IV18_Set_Dig(7,BSP_IV18_BLANK);    
  } else {
    BSP_IV18_Set_Dig(5,'O');
    BSP_IV18_Set_Dig(6,'F');
    BSP_IV18_Set_Dig(7,'F');      
  }
  BSP_IV18_Set_Blink(5);
  BSP_IV18_Set_Blink(6); 
  BSP_IV18_Set_Blink(7);
}

void display_format_player_vol(void)
{
  BSP_IV18_Set_Dig(1,'P');
  BSP_IV18_Set_Dig(2,'L');
  BSP_IV18_Set_Dig(3,'Y');
  BSP_IV18_Set_Dig(4,'V');
  BSP_IV18_Set_Dig(5,'O');
  BSP_IV18_Set_Dig(6,'L'); 
  BSP_IV18_Set_Dig(7, player_get_vol() / 10 + 0x30);
  BSP_IV18_Set_Dig(8, player_get_vol() % 10 + 0x30); 
  
  BSP_IV18_Set_Blink(7); 
  BSP_IV18_Set_Blink(8);
}

void display_format_motion_mon(void)
{
  BSP_IV18_Set_Dig(2,'M');
  BSP_IV18_Set_Dig(3,'M');
  BSP_IV18_Set_Dig(4,'-');
  if(!motion_sensor_test_enable()) {
    BSP_IV18_Set_Dig(5,'O');
    BSP_IV18_Set_Dig(6,'F');
    BSP_IV18_Set_Dig(7,'F');    
  } else {
    BSP_IV18_Set_Dig(5, 'O');
    BSP_IV18_Set_Dig(6, 'N');
    BSP_IV18_Set_Dig(7, BSP_IV18_BLANK);      
  }
  BSP_IV18_Set_Blink(5);
  BSP_IV18_Set_Blink(6); 
  BSP_IV18_Set_Blink(7);
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
  _display_mon_light = config_read_int("mon_lt_en");
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

void display_show_string(uint8_t index, const char * str)
{
  uint32_t len = strlen(str);
  uint8_t i;
  for(i = 0 ; i < len ; i++) {
    if(index > 8)
      return;
    BSP_IV18_Set_Dig(index ++, str[i]);
  }
}

void display_set_dig(uint8_t index, uint8_t code)
{
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
  uint16_t lt_0 = config_read_int("lt_0");
  uint16_t lt_100 = config_read_int("lt_100"); 
  double k, b;
  int8_t percent;
  k = ((double)(100 - 0)) / ((double)(lt_100 - lt_0));
  b = 100 - k * lt_100;
  percent = (int8_t)(k * val + b); 
  IVDBG("display_get_light_percent: val = %d, k = %f, b = %f, v = %d", val, k, b, percent);
  if(percent > 100) {
    percent = 100;
  }
  if(percent < 0) {
    percent = 0;
  }
  return (uint8_t)percent;
}

void display_mon_light_set_enable(bool enable)
{
  
  _display_mon_light = enable;
  if(enable)
    BSP_TIM1_Start_PMW(TIM_CHANNEL_1);
  else
    BSP_TIM1_Stop_PMW(TIM_CHANNEL_1);
}

bool display_mon_light_test_enable(void)
{
  return _display_mon_light;
}

void display_mon_light_save_config(void)
{
  config_val_t val;
  val.val8 = _display_mon_light;
  config_write("mon_lt_en", &val);
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

void display_set_blink_alarm_sel(bool enable)
{
  if(enable) {
    display_set_blink(3); 
  } else {
    display_clr_blink(3);
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
    display_set_blink(8);    
  } else {
    display_clr_blink(8); 
  }
}
