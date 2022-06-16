#ifndef __IVCLOCK_PWR_H__
#define __IVCLOCK_PWR_H__

#include <stdint.h>
#include "bsp.h"
#include "cext.h"

BSP_Error_Type BSP_PWR_Init(void);
BSP_Error_Type BSP_PWR_SystemClock_Config(void);
void BSP_PWR_Sleep(void);

void BSP_GPIO_PWR_33_Enable(FlagStatus enable);
void BSP_GPIO_PWR_50_Enable(FlagStatus enable);
void BSP_GPIO_PWR_490_Enable(FlagStatus enable);
void BSP_GPIO_PWR_IV18_Enable(FlagStatus enable);
bool BSP_GPIO_PWR_33_Enabled(void);
bool BSP_GPIO_PWR_50_Enabled(void);
bool BSP_GPIO_PWR_490_Enabled(void);
bool BSP_GPIO_PWR_IV18_Enabled(void);

#endif