#ifndef __IVCLOCK_GPIO_H__
#define __IVCLOCK_GPIO_H__

#include <stdint.h>
#include "stm32f1xx_hal.h"

int8_t BSP_GPIO_Init(void);
void BSP_GPIO_Power_33_Enable(FlagStatus enable);
void BSP_GPIO_Power_50_Enable(FlagStatus enable);
void BSP_GPIO_Power_490_Enable(FlagStatus enable);
#endif