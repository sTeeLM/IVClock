#include "pwr.h"
#include "bsp.h"
#include "debug.h"

BSP_Error_Type BSP_PWR_Init(void)
{
  return BSP_ERROR_NONE;
}
void SystemClock_Config(void);

void BSP_PWR_Sleep(void)
{
  HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
  SystemClock_Config();
}
