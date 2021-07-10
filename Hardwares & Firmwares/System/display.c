#include "display.h"
#include "iv18.h"
#include "debug.h"
#include "power.h"
#include "tim.h"
#include "adc.h"

static bool _display_is_on;
  
void display_init(void)
{
  _display_is_on = FALSE;
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
  BSP_TIM3_Start(); 
  BSP_TIM4_Start();
  BSP_ADC2_Start();
  _display_is_on = TRUE;
}

void display_off(void)
{
  power_iv18_enable(FALSE);
  power_490_enable(FALSE);
  BSP_TIM3_Stop(); 
  BSP_TIM4_Stop();
  BSP_ADC2_Stop();
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

void display_set_light(uint8_t percent)
{
  if(percent > 100)
    percent = 100;
  if(percent == 0 )
    percent = 1;
  BSP_TIM3_Set_Duty_Cycle(100 - percent);
}
