#include "beeper.h"
#include "tim.h"
#include "delay.h"
#include "config.h"

void beeper_init(void)
{
  BSP_TIM1_Stop_PMW(TIM_CHANNEL_2);
}

void beeper_beep(void)
{
  if(beeper_enabled()) {
    BSP_TIM1_Start_Beeper(64, 0);
  }
}

void beeper_beep_beep(void)
{
  if(beeper_enabled()) {
    BSP_TIM1_Start_Beeper(84, 1); 
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