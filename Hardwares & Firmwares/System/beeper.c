#include "beeper.h"
#include "tim.h"
#include "delay.h"
void beeper_init(void)
{

}

void beeper_beep(void)
{
	BSP_TIM1_Start();
  delay_ms(40);
  BSP_TIM1_Stop();
}

void beeper_beep_beep(void)
{
	BSP_TIM1_Start();
  delay_ms(40);
  BSP_TIM1_Stop();
  delay_ms(30);
	BSP_TIM1_Start();
  delay_ms(40);  
  BSP_TIM1_Stop();  
}