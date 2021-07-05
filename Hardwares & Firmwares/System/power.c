#include "power.h"
#include "cext.h"
#include "debug.h"
#include "gpio.h"
#include "adc.h"

void power_init(void)
{
  power_50_enable(FALSE);
  power_490_enable(FALSE);
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



