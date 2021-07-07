#ifndef __IVCLOCK_GPIO_H__
#define __IVCLOCK_GPIO_H__

#include <stdint.h>
#include "bsp.h"
#include "cext.h"

BSP_Error_Type BSP_GPIO_Init(void);
void BSP_GPIO_Power_33_Enable(FlagStatus enable);
void BSP_GPIO_Power_50_Enable(FlagStatus enable);
void BSP_GPIO_Power_490_Enable(FlagStatus enable);
bool BSP_GPIO_Power_33_Enabled(void);
bool BSP_GPIO_Power_50_Enabled(void);
bool BSP_GPIO_Power_490_Enabled(void);

#endif