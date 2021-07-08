#include "beeper.h"
#include "tim.h"
#include "delay.h"
#include "config.h"

void beeper_init(void)
{

}

void beeper_beep(void)
{
  if(beeper_enabled()) {
    BSP_TIM1_Start();
    delay_ms(40);
    BSP_TIM1_Stop();
  }
}

void beeper_beep_beep(void)
{
  if(beeper_enabled()) {
    BSP_TIM1_Start();
    delay_ms(40);
    BSP_TIM1_Stop();
    delay_ms(30);
    BSP_TIM1_Start();
    delay_ms(40);  
    BSP_TIM1_Stop(); 
  }   
}

void beeper_enable(bool enable)
{
  config_val_t val;
  val.val8 = enable ? 1 : 0;
  config_write("bp_on", &val);
}

bool beeper_enabled(void)
{
  return config_read("bp_on")->val8;
}