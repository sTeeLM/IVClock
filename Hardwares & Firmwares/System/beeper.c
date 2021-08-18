#include "beeper.h"
#include "tim.h"
#include "delay.h"
#include "config.h"

static bool beeper_enabled;

void beeper_init(void)
{
  BSP_TIM1_Stop_PMW(TIM_CHANNEL_2);
  beeper_enabled = config_read_int("bp_en");
}

void beeper_beep(void)
{
  if(beeper_enabled) {
    BSP_TIM1_Start_Beeper(64, 0);
  }
}

void beeper_beep_beep(void)
{
  if(beeper_enabled) {
    BSP_TIM1_Start_Beeper(128, 1); 
  }   
}

void beeper_set_enable(bool enable)
{
  beeper_enabled = enable;
}

bool beeper_test_enable(void)
{
  return beeper_enabled;
}

void beeper_save_config(void)
{
  config_val_t val;
  val.val8 = beeper_enabled ? 1 : 0;
  config_write("bp_en", &val);  
}